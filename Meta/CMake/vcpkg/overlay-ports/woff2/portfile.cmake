vcpkg_check_linkage(
  ONLY_STATIC_LIBRARY
)

vcpkg_from_github(
  OUT_SOURCE_PATH SOURCE_PATH
  REPO google/woff2
  REF v1.0.2
  SHA512 c788bba1530aec463e755e901f9342f4b599e3a07f54645fef1dc388ab5d5c30625535e5dd38e9e792e04a640574baa50eeefb6b7338ab403755f4a4e0c3044d
  HEAD_REF master
  PATCHES
    0001-unofficial-brotli.patch
    0002-stdint-include.patch
)

vcpkg_cmake_configure(
  SOURCE_PATH "${SOURCE_PATH}"
  OPTIONS
    -DCANONICAL_PREFIXES=ON
    "-Dunofficial-brotli_DIR=${CURRENT_INSTALLED_DIR}/share/unofficial-brotli"
  MAYBE_UNUSED_VARIABLES
    unofficial-brotli_DIR
)

vcpkg_cmake_install()
vcpkg_copy_pdbs()

# vcpkg_cmake_install() uses CMake's install/strip path for release builds. For
# static ARMHF archives that removes the symbol table required by GNU ld, so copy
# the freshly built archives back after install.
foreach(build_type IN ITEMS rel dbg)
  if(build_type STREQUAL "dbg")
    set(woff2_archive_destination "${CURRENT_PACKAGES_DIR}/debug/lib")
  else()
    set(woff2_archive_destination "${CURRENT_PACKAGES_DIR}/lib")
  endif()

  file(GLOB woff2_build_dirs LIST_DIRECTORIES true "${CURRENT_BUILDTREES_DIR}/${TARGET_TRIPLET}-*-${build_type}")
  foreach(woff2_build_dir IN LISTS woff2_build_dirs)
    foreach(woff2_archive IN ITEMS libwoff2common.a libwoff2dec.a libwoff2enc.a)
      if(EXISTS "${woff2_build_dir}/${woff2_archive}")
        file(COPY_FILE "${woff2_build_dir}/${woff2_archive}" "${woff2_archive_destination}/${woff2_archive}")
      endif()
    endforeach()
  endforeach()
endforeach()

file(COPY "${CURRENT_PACKAGES_DIR}/bin/" DESTINATION "${CURRENT_PACKAGES_DIR}/tools/woff2")
file(REMOVE_RECURSE
  "${CURRENT_PACKAGES_DIR}/bin"
  "${CURRENT_PACKAGES_DIR}/debug/bin"
  "${CURRENT_PACKAGES_DIR}/debug/include"
)

vcpkg_copy_tool_dependencies("${CURRENT_PACKAGES_DIR}/tools/woff2")

vcpkg_fixup_pkgconfig()
# Handle copyright
file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/woff2" RENAME copyright)
