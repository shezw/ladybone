set(JINGWEI_SOURCE_DIR "${LADYBIRD_SOURCE_DIR}/../JingWei" CACHE PATH "Path to the JingWei source tree")

if (NOT TARGET jingwei)
    if (NOT EXISTS "${JINGWEI_SOURCE_DIR}/CMakeLists.txt")
        message(FATAL_ERROR "LADYBIRD_GUI_FRAMEWORK=JW requires JINGWEI_SOURCE_DIR to point at a JingWei checkout")
    endif()

    set(JINGWEI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(JINGWEI_BUILD_PLAYGROUND OFF CACHE BOOL "" FORCE)
    add_subdirectory("${JINGWEI_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/JingWei" EXCLUDE_FROM_ALL)
endif()

add_executable(ladybird main.cpp)
target_sources(ladybird PRIVATE
    Application.cpp
    WebContentView.cpp
)
target_link_libraries(ladybird PRIVATE jingwei)
