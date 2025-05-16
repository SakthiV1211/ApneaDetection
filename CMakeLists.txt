cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(ExeOut VERSION 0.1.0)

# file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/Algorithm/Src/*.cpp)
# message("${SRC_FILES}")
add_compile_definitions(AHI_LOG_DEGUB=1)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ResmedFlow.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ResmedFlow.txt ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

message("BUFF")
message(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME} main.cpp)

LIST(APPEND APP_INC ${PROJECT_SOURCE_DIR}/Algorithm)

FOREACH(i ${APP_INC})
message ("${i}")
ENDFOREACH()

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/Algorithm/Inc)

add_subdirectory(${PROJECT_SOURCE_DIR}/Algorithm)
target_include_directories(${PROJECT_NAME} PUBLIC ${APP_INC})

target_link_directories(${PROJECT_NAME} PUBLIC ${APP_INC})


target_link_libraries(${PROJECT_NAME} ALGO_Lib)
