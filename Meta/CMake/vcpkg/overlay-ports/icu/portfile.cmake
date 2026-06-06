vcpkg_download_distfile(
    ARCHIVE
    URLS "https://github.com/unicode-org/icu/releases/download/release-${VERSION}/icu4c-${VERSION}-sources.tgz"
    FILENAME "icu4c-${VERSION}-sources.tgz"
    SHA512 92feddfe81c57336f386c7cbc9f6d976bf349db148a77a247c4559676f51116115c8c52c4d907feb50933f72ab75fd8e48be092bf9c8ca33a3e8fabc9372a5d6
)

vcpkg_extract_source_archive(SOURCE_PATH
    ARCHIVE "${ARCHIVE}"
    PATCHES
        disable-static-prefix.patch # https://gitlab.kitware.com/cmake/cmake/-/issues/16617; also mingw.
        fix_bsd_and_solaris.patch
        fix_parallel_build_on_windows.patch
        mh-darwin.patch
        mh-mingw.patch
        mh-msys-msvc.patch
        subdirs.patch
        vcpkg-cross-data.patch
)

vcpkg_find_acquire_program(PYTHON3)
set(ENV{PYTHON} "${PYTHON3}")

vcpkg_list(SET CONFIGURE_OPTIONS)
vcpkg_list(SET BUILD_OPTIONS)

if(VCPKG_TARGET_IS_EMSCRIPTEN)
    vcpkg_list(APPEND CONFIGURE_OPTIONS --disable-extras icu_cv_host_frag=mh-linux)
    vcpkg_list(APPEND BUILD_OPTIONS "\"PKGDATA_OPTS=--without-assembly -O ../data/icupkg.inc\"")
elseif(VCPKG_TARGET_IS_UWP)
    vcpkg_list(APPEND CONFIGURE_OPTIONS --disable-extras ac_cv_func_tzset=no ac_cv_func__tzset=no)
    string(APPEND VCPKG_C_FLAGS " -DU_PLATFORM_HAS_WINUWP_API=1")
    string(APPEND VCPKG_CXX_FLAGS " -DU_PLATFORM_HAS_WINUWP_API=1")
    vcpkg_list(APPEND BUILD_OPTIONS "\"PKGDATA_OPTS=--windows-uwp-build -O ../data/icupkg.inc\"")
elseif(VCPKG_TARGET_IS_OSX AND VCPKG_LIBRARY_LINKAGE STREQUAL "dynamic")
    vcpkg_list(APPEND CONFIGURE_OPTIONS --enable-rpath)
    if(DEFINED CMAKE_INSTALL_NAME_DIR)
        vcpkg_list(APPEND BUILD_OPTIONS "ID_PREFIX=${CMAKE_INSTALL_NAME_DIR}")
    endif()
endif()

if(VCPKG_TARGET_IS_WINDOWS AND NOT VCPKG_TARGET_IS_MINGW)
    list(APPEND CONFIGURE_OPTIONS ac_cv_lib_m_floor=no)
endif()

if("tools" IN_LIST FEATURES)
  list(APPEND CONFIGURE_OPTIONS --enable-tools)
else()
  list(APPEND CONFIGURE_OPTIONS --disable-tools)
endif()
if(CMAKE_HOST_WIN32 AND VCPKG_TARGET_IS_MINGW AND NOT HOST_TRIPLET MATCHES "mingw")
    # Assuming no cross compiling because the host (windows) pkgdata tool doesn't
    # use the '/' path separator when creating compiler commands for mingw bash.
elseif(VCPKG_CROSSCOMPILING)
    set(TOOL_PATH "${CURRENT_HOST_INSTALLED_DIR}/tools/${PORT}")
    # convert to unix path
    string(REGEX REPLACE "^([a-zA-Z]):/" "/\\1/" _VCPKG_TOOL_PATH "${TOOL_PATH}")
    list(APPEND CONFIGURE_OPTIONS "--with-cross-build=${_VCPKG_TOOL_PATH}")
endif()

vcpkg_make_configure(
    SOURCE_PATH "${SOURCE_PATH}/source"
    # AUTORECONF # needs Autoconf version 2.72
    OPTIONS
        ${CONFIGURE_OPTIONS}
        --disable-samples
        --disable-tests
        --disable-layoutex
    OPTIONS_RELEASE
        --disable-debug
        --enable-release
    OPTIONS_DEBUG
        --enable-debug
        --disable-release
)
vcpkg_make_install(OPTIONS ${BUILD_OPTIONS})

file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/share"
    "${CURRENT_PACKAGES_DIR}/debug/share"
    "${CURRENT_PACKAGES_DIR}/lib/icu"
    "${CURRENT_PACKAGES_DIR}/debug/lib/icu"
    "${CURRENT_PACKAGES_DIR}/debug/lib/icud")

file(GLOB TEST_LIBS
    "${CURRENT_PACKAGES_DIR}/lib/*test*"
    "${CURRENT_PACKAGES_DIR}/debug/lib/*test*")
if(TEST_LIBS)
    file(REMOVE ${TEST_LIBS})
endif()

if(NOT "tools" IN_LIST FEATURES)
    set(ICU_PREBUILT_DATA "${SOURCE_PATH}/source/data/in/icudt78l.dat")
    set(ICU_FILTERED_DATA_DIR "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-filtered-data")
    set(ICU_REMOVE_LIST "${ICU_FILTERED_DATA_DIR}/remove-list.txt")
    set(ICU_FILTERED_DATA "${ICU_FILTERED_DATA_DIR}/icudt78l.dat")
    set(ICU_FILTER_SCRIPT "${ICU_FILTERED_DATA_DIR}/filter-icu-data.py")
    set(ICU_HOST_ICUPKG "${CURRENT_HOST_INSTALLED_DIR}/tools/icu/bin/icupkg${VCPKG_HOST_EXECUTABLE_SUFFIX}")

    file(MAKE_DIRECTORY "${ICU_FILTERED_DATA_DIR}")
    file(WRITE "${ICU_FILTER_SCRIPT}" [=[
import json
import re
import subprocess
import sys
from pathlib import Path

icupkg, source_dat, filter_json, remove_list = map(Path, sys.argv[1:])
config = json.loads(filter_json.read_text())
locales = set(config["localeFilter"]["includelist"])
locales.update({"root", "en_US_POSIX"})

locale_trees = {"coll", "curr", "lang", "rbnf", "region", "unit", "zone"}
brkitr_keep = {
    "burmesedict.dict",
    "char.brk",
    "cjdict.dict",
    "en.res",
    "en_US.res",
    "en_US_POSIX.res",
    "khmerdict.dict",
    "laodict.dict",
    "line.brk",
    "line_cj.brk",
    "line_loose.brk",
    "line_loose_cj.brk",
    "line_normal.brk",
    "line_normal_cj.brk",
    "res_index.res",
    "root.res",
    "sent.brk",
    "thaidict.dict",
    "title.brk",
    "word.brk",
    "word_POSIX.brk",
    "zh.res",
}
converter_keep_patterns = [
    re.compile(pattern)
    for pattern in [
        r"^ascii.*\.cnv$",
        r"^gb.*\.cnv$",
        r"^big5.*\.cnv$",
        r"^ibm-367.*\.cnv$",
        r"^ibm-819.*\.cnv$",
        r"^ibm-120[0-9].*\.cnv$",
        r"^ibm-1252.*\.cnv$",
        r"^iso-8859_1.*\.cnv$",
        r"^utf.*\.cnv$",
    ]
]

def keep_locale_resource(name: str) -> bool:
    if not name.endswith(".res"):
        return True
    stem = name[:-4]
    return stem in locales or stem in {"pool", "res_index"}

def keep_item(item: str) -> bool:
    if "/" in item:
        tree, name = item.split("/", 1)
        if tree in locale_trees:
            return keep_locale_resource(name)
        if tree == "brkitr":
            return name in brkitr_keep
        if tree == "translit":
            return name in {"root.res", "en.res"}
        return True

    if item.endswith(".cnv"):
        return any(pattern.match(item) for pattern in converter_keep_patterns)
    if item.endswith(".res"):
        return keep_locale_resource(item)
    return True

items = subprocess.check_output([str(icupkg), "-l", str(source_dat)], text=True).splitlines()
remove = [item for item in items if item and not keep_item(item)]
remove_list.write_text("\n".join(remove) + ("\n" if remove else ""))
]=])

    vcpkg_execute_required_process(
        COMMAND "${PYTHON3}" "${ICU_FILTER_SCRIPT}" "${ICU_HOST_ICUPKG}" "${ICU_PREBUILT_DATA}" "${CMAKE_CURRENT_LIST_DIR}/ladybone-data-filter.json" "${ICU_REMOVE_LIST}"
        WORKING_DIRECTORY "${ICU_FILTERED_DATA_DIR}"
        LOGNAME "filter-icu-data-${TARGET_TRIPLET}"
    )
    vcpkg_execute_required_process(
        COMMAND "${ICU_HOST_ICUPKG}" -r "${ICU_REMOVE_LIST}" "${ICU_PREBUILT_DATA}" "${ICU_FILTERED_DATA}"
        WORKING_DIRECTORY "${ICU_FILTERED_DATA_DIR}"
        LOGNAME "pack-filtered-icu-data-${TARGET_TRIPLET}"
    )

    file(INSTALL "${ICU_FILTERED_DATA}" DESTINATION "${CURRENT_PACKAGES_DIR}/share/icu/${VERSION}")

    set(ICU_STUBDATA_LIBRARY "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel/stubdata/${VCPKG_TARGET_SHARED_LIBRARY_PREFIX}icudata${VCPKG_TARGET_SHARED_LIBRARY_SUFFIX}.${VERSION}")
    if(EXISTS "${ICU_STUBDATA_LIBRARY}")
        file(COPY_FILE "${ICU_STUBDATA_LIBRARY}" "${CURRENT_PACKAGES_DIR}/lib/${VCPKG_TARGET_SHARED_LIBRARY_PREFIX}icudata${VCPKG_TARGET_SHARED_LIBRARY_SUFFIX}.${VERSION}")
    endif()
endif()

if(VCPKG_LIBRARY_LINKAGE STREQUAL "static")
    # force U_STATIC_IMPLEMENTATION macro
    foreach(HEADER utypes.h utf_old.h platform.h)
        vcpkg_replace_string("${CURRENT_PACKAGES_DIR}/include/unicode/${HEADER}" "defined(U_STATIC_IMPLEMENTATION)" "1")
    endforeach()
endif()

# Install executables from /tools/icu/sbin to /tools/icu/bin on unix (/bin because icu require this for cross compiling)
if(VCPKG_TARGET_IS_LINUX OR VCPKG_TARGET_IS_OSX AND "tools" IN_LIST FEATURES)
    vcpkg_copy_tools(
        TOOL_NAMES icupkg gennorm2 gencmn genccode gensprep
        SEARCH_DIR "${CURRENT_PACKAGES_DIR}/tools/icu/sbin"
        DESTINATION "${CURRENT_PACKAGES_DIR}/tools/${PORT}/bin"
    )
endif()

file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/tools/icu/sbin"
    "${CURRENT_PACKAGES_DIR}/tools/icu/debug")

# To cross compile, we need some files at specific positions. So lets copy them
file(GLOB CROSS_COMPILE_DEFS "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-rel/config/icucross.*")
file(INSTALL ${CROSS_COMPILE_DEFS} DESTINATION "${CURRENT_PACKAGES_DIR}/tools/${PORT}/config")

if(VCPKG_TARGET_IS_WINDOWS)
    string(REGEX MATCH "^[0-9]*" ICU_VERSION_MAJOR "${VERSION}")
    file(GLOB RELEASE_DLLS "${CURRENT_PACKAGES_DIR}/lib/*icu*${ICU_VERSION_MAJOR}.dll")
    file(COPY ${RELEASE_DLLS} DESTINATION "${CURRENT_PACKAGES_DIR}/tools/${PORT}/bin")

    # copy dlls
    file(GLOB RELEASE_DLLS "${CURRENT_PACKAGES_DIR}/lib/*icu*${ICU_VERSION_MAJOR}.dll")
    file(COPY ${RELEASE_DLLS} DESTINATION "${CURRENT_PACKAGES_DIR}/bin")
    if(NOT VCPKG_BUILD_TYPE)
        file(GLOB DEBUG_DLLS "${CURRENT_PACKAGES_DIR}/debug/lib/*icu*${ICU_VERSION_MAJOR}.dll")
        file(COPY ${DEBUG_DLLS} DESTINATION "${CURRENT_PACKAGES_DIR}/debug/bin")
    endif()

    # remove any remaining dlls in /lib
    file(GLOB DUMMY_DLLS "${CURRENT_PACKAGES_DIR}/lib/*.dll" "${CURRENT_PACKAGES_DIR}/debug/lib/*.dll")
    if(DUMMY_DLLS)
        file(REMOVE ${DUMMY_DLLS})
    endif()

    vcpkg_copy_pdbs()
endif()

vcpkg_fixup_pkgconfig()
set(cxx_link_libraries "")
if(VCPKG_LIBRARY_LINKAGE STREQUAL "static")
    block(PROPAGATE cxx_link_libraries)
        vcpkg_cmake_get_vars(cmake_vars_file)
        include("${cmake_vars_file}")
        list(REMOVE_ITEM VCPKG_DETECTED_CMAKE_CXX_IMPLICIT_LINK_LIBRARIES ${VCPKG_DETECTED_CMAKE_C_IMPLICIT_LINK_LIBRARIES})
        list(TRANSFORM VCPKG_DETECTED_CMAKE_CXX_IMPLICIT_LINK_LIBRARIES REPLACE "^([^/]+)\$" "-l\\1")
        string(JOIN " " cxx_link_libraries ${VCPKG_DETECTED_CMAKE_CXX_IMPLICIT_LINK_LIBRARIES})
    endblock()
    vcpkg_replace_string("${CURRENT_PACKAGES_DIR}/lib/pkgconfig/icu-uc.pc" "baselibs = " "baselibs = ${cxx_link_libraries} ")
    if(NOT VCPKG_BUILD_TYPE)
        vcpkg_replace_string("${CURRENT_PACKAGES_DIR}/debug/lib/pkgconfig/icu-uc.pc" "baselibs = " "baselibs = ${cxx_link_libraries} ")
    endif()
endif()

vcpkg_replace_string("${CURRENT_PACKAGES_DIR}/tools/icu/bin/icu-config" "${CURRENT_INSTALLED_DIR}" "`dirname $0`/../../../" IGNORE_UNCHANGED)
vcpkg_replace_string("${CURRENT_PACKAGES_DIR}/tools/icu/bin/icu-config" "${CURRENT_HOST_INSTALLED_DIR}" "`dirname $0`/../../../../${_HOST_TRIPLET}/" IGNORE_UNCHANGED)

configure_file("${CMAKE_CURRENT_LIST_DIR}/vcpkg-cmake-wrapper.cmake" "${CURRENT_PACKAGES_DIR}/share/${PORT}/vcpkg-cmake-wrapper.cmake" @ONLY)
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
