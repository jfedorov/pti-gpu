if (UNIX)
  set(CMAKE_C_COMPILER $ENV{CMPLR_ROOT}/linux/bin/icx)
  set(CMAKE_CXX_COMPILER $ENV{CMPLR_ROOT}/linux/bin/icpx)
endif()
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-fsanitize=address,undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_C_FLAGS_DEBUG_INIT "-fsanitize=address,undefined")
