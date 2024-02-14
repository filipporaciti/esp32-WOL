# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)



if(DEFINED ENV{RMAKER_PATH})
  set(RMAKER_PATH $ENV{RMAKER_PATH})
else()
  set(RMAKER_PATH ${CMAKE_CURRENT_LIST_DIR}/esp-rainmaker)
endif(DEFINED ENV{RMAKER_PATH})

# Add RainMaker components and other common application components
set(EXTRA_COMPONENT_DIRS ${RMAKER_PATH}/components/esp-insights/components ${RMAKER_PATH}/components ${RMAKER_PATH}/examples/common)



include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32-WOL)