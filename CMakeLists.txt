cmake_minimum_required(VERSION 3.8)
project(CrystalBot LANGUAGES CXX)

add_subdirectory(RLBotBMCPP)
add_subdirectory(RLUtilities)

file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/RLBotBMCPP/RLBotBM/src/shared/*.cpp)

add_executable(CrystalBot ${sources})
target_include_directories(CrystalBot PUBLIC src)

target_include_directories(CrystalBot PRIVATE RLBotBMCPP/inc RLBotBMCPP/lib/inc)
target_include_directories(CrystalBot PRIVATE RLUtilities/inc RLUtilities/inc/*)

if (UNIX)
    target_link_libraries(CrystalBot RLBotBMCPP pthread)
else (UNIX)
    target_link_libraries(CrystalBot RLBotBMCPP)
endif (UNIX)

target_link_libraries(CrystalBot rlutilities_cpp)

target_compile_features(CrystalBot PRIVATE cxx_std_20)

# Copy port config to output folder
add_custom_command(TARGET CrystalBot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/port.cfg"
        $<TARGET_FILE_DIR:CrystalBot>)

# Copy port config to release folder
add_custom_command(TARGET CrystalBot POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/port.cfg"
        ${PROJECT_SOURCE_DIR}/rlbot)

# Copy assets to executable folder
add_custom_command(TARGET CrystalBot POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/RLUtilities/assets"
        $<TARGET_FILE_DIR:CrystalBot>/assets)

# Copy assets to release folder
file(COPY ${PROJECT_SOURCE_DIR}/RLUtilities/assets DESTINATION ${PROJECT_SOURCE_DIR}/rlbot)