cmake_minimum_required(VERSION 3.8)
project(Orbuculum LANGUAGES CXX)

add_subdirectory(RLBotBMCPP)
add_subdirectory(RLUtilities)

file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/RLBotBMCPP/RLBotBM/src/shared/*.cpp)

add_executable(Orbuculum ${sources})
target_include_directories(Orbuculum PUBLIC src)

target_include_directories(Orbuculum PRIVATE RLBotBMCPP/inc RLBotBMCPP/lib/inc)
target_include_directories(Orbuculum PRIVATE RLUtilities/inc RLUtilities/inc/*)

if (UNIX)
    target_link_libraries(Orbuculum RLBotBMCPP pthread)
else (UNIX)
    target_link_libraries(Orbuculum RLBotBMCPP)
endif (UNIX)

target_link_libraries(Orbuculum rlutilities_cpp)

target_compile_features(Orbuculum PRIVATE cxx_std_17)

# Copy port config to output folder
add_custom_command(TARGET Orbuculum POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/port.cfg"
        $<TARGET_FILE_DIR:Orbuculum>)

# Copy port config to release folder
add_custom_command(TARGET Orbuculum POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/port.cfg"
        ${PROJECT_SOURCE_DIR}/rlbot)

# Copy assets to executable folder
add_custom_command(TARGET Orbuculum POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/RLUtilities/assets"
        $<TARGET_FILE_DIR:Orbuculum>/assets)

# Copy assets to release folder
file(COPY ${PROJECT_SOURCE_DIR}/RLUtilities/assets DESTINATION ${PROJECT_SOURCE_DIR}/rlbot)