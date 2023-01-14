cmake_minimum_required(VERSION 3.13.1)

set(MODEL_FOLDER source)
set(EI_SDK_FOLDER source/edge-impulse-sdk)

include(pico_sdk_import.cmake)

project(NerfBarrel C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

pico_sdk_init()

add_executable(${PROJECT_NAME}
  source/mpu6050.cpp
  source/main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC inc/)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})

include(${MODEL_FOLDER}/edge-impulse-sdk/cmake/utils.cmake)

# add required libraries
target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_gpio hardware_i2c hardware_pio hardware_pwm pico_multicore)

# add required dirs
target_include_directories(${PROJECT_NAME} PRIVATE
  ${MODEL_FOLDER}
  ${MODEL_FOLDER}/classifer
  ${MODEL_FOLDER}/tflite-model
  ${MODEL_FOLDER}/model-parameters
  )

target_include_directories(${PROJECT_NAME} PRIVATE
  ${EI_SDK_FOLDER}
  ${EI_SDK_FOLDER}/third_party/ruy
  ${EI_SDK_FOLDER}/third_party/gemmlowp
  ${EI_SDK_FOLDER}/third_party/flatbuffers/include
  ${EI_SDK_FOLDER}/third_party
  ${EI_SDK_FOLDER}/tensorflow
  ${EI_SDK_FOLDER}/dsp
  ${EI_SDK_FOLDER}/classifier
  ${EI_SDK_FOLDER}/anomaly
  ${EI_SDK_FOLDER}/CMSIS/NN/Include
  ${EI_SDK_FOLDER}/CMSIS/DSP/PrivateInclude
  ${EI_SDK_FOLDER}/CMSIS/DSP/Include
  ${EI_SDK_FOLDER}/CMSIS/Core/Include
  )

# add models & sdk sources
RECURSIVE_FIND_FILE(MODEL_FILES "${MODEL_FOLDER}/tflite-model" "*.cpp")
RECURSIVE_FIND_FILE(SOURCE_FILES "${EI_SDK_FOLDER}" "*.cpp")
RECURSIVE_FIND_FILE(CC_FILES "${EI_SDK_FOLDER}" "*.cc")
RECURSIVE_FIND_FILE(S_FILES "${EI_SDK_FOLDER}" "*.s")
RECURSIVE_FIND_FILE(C_FILES "${EI_SDK_FOLDER}" "*.c")
list(APPEND SOURCE_FILES ${S_FILES})
list(APPEND SOURCE_FILES ${C_FILES})
list(APPEND SOURCE_FILES ${CC_FILES})
list(APPEND SOURCE_FILES ${MODEL_FILES})

# add all sources to the project
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})