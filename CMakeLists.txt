cmake_minimum_required(VERSION 3.20.0)
project(cppShinyRoamer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(cppShinyRoamer 
    main.cpp
    WindowHandler/WindowHandler.cpp
    Detection/Detection.cpp 
    SerialHandler/SerialHandler.cpp
)

add_custom_command(TARGET cppShinyRoamer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/tree_reference.png
        $<TARGET_FILE_DIR:cppShinyRoamer>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/roamer_reference.png
        $<TARGET_FILE_DIR:cppShinyRoamer>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/raikou_normal.png
        $<TARGET_FILE_DIR:cppShinyRoamer>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/raikou_shiny.png
        $<TARGET_FILE_DIR:cppShinyRoamer>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/entei_normal.png
        $<TARGET_FILE_DIR:cppShinyRoamer>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/entei_shiny.png
        $<TARGET_FILE_DIR:cppShinyRoamer>
)

target_include_directories(cppShinyRoamer PRIVATE 
    WindowHandler
    SerialHandler
    Detection
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(cppShinyRoamer PRIVATE ${OpenCV_LIBS})
