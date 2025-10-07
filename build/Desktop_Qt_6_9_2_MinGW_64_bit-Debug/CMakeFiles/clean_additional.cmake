# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\menuges_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\menuges_autogen.dir\\ParseCache.txt"
  "menuges_autogen"
  )
endif()
