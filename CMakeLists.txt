cmake_minimum_required(VERSION 3.10)
project(main)

set(CMAKE_CXX_STANDARD 14)

# PARA TENSORFLOW #
set(REQUIRED_LIBS ${TFLITE_LIBS})
find_package(OpenCV 4 REQUIRED)
find_package(Threads REQUIRED)
find_library(MOSQUITTO_LIBS mosquitto "lib")
find_library(TFLITE_LIBRARY tensorflow-lite "lib")
find_library(FLATBUFFER_LIBRARY flatbuffers "lib")

include_directories("include")

set(REQUIRED_LIBS "stdc++fs")
list(APPEND REQUIRED_LIBS ${TFLITE_LIBRARY})
list(APPEND REQUIRED_LIBS ${FLATBUFFER_LIBRARY})
list(APPEND REQUIRED_LIBS ${OpenCV_LIBS})
list(APPEND REQUIRED_LIBS ${Threads_LIBS})
list(APPEND REQUIRED_LIBS ${MOSQUITTO_LIBS})

set(CMAKE_C_FLAGS "-pthread")

add_executable("main"
        src/main.cpp
        src/centroidtracker.cpp
        src/publisher.cpp
        src/image_ops.cpp
        include/publisher.h
        include/image_ops.h
        include/heatmap.h
        include/tensorflow_clss.h
        include/centroidtracker.h)

SET_TARGET_PROPERTIES(main PROPERTIES LINK_FLAGS "-no-pie")
target_link_libraries("main" ${REQUIRED_LIBS} -lpthread -ldl -lmosquitto)
