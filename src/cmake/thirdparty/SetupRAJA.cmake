# Copyright (c) Lawrence Livermore National Security, LLC and other Ascent
# Project developers. See top-level LICENSE AND COPYRIGHT files for dates and
# other details. No copyright assignment is required to contribute to Ascent.

if(NOT RAJA_DIR)
  message(FATAL_ERROR "RAJA support needs explicit RAJA_DIR")
endif()

message(STATUS "Looking for RAJA in: ${RAJA_DIR}")

set(_RAJA_SEARCH_PATH)
if(EXISTS ${RAJA_DIR}/share/raja/cmake)
  # old install layout
  set(_RAJA_SEARCH_PATH ${RAJA_DIR}/share/raja/cmake)
elseif(EXISTS ${RAJA_DIR}/lib/cmake/raja)
  # new install layout
  set(_RAJA_SEARCH_PATH ${RAJA_DIR}/lib/cmake/raja)
else ()
  # try RAJA_DIR itself
  set(_RAJA_SEARCH_PATH ${RAJA_DIR})
endif()

message(STATUS "Looking for RAJA in: ${RAJA_DIR}")

set(RAJA_DIR_ORIG ${RAJA_DIR})
find_dependency(RAJA REQUIRED
                NO_DEFAULT_PATH
                PATHS ${_RAJA_SEARCH_PATH})
message(STATUS "Found RAJA in: ${RAJA_DIR}")


if(ASCENT_ENABLE_TESTS AND WIN32 AND BUILD_SHARED_LIBS)
    # if we are running tests with dlls, we need path to dlls
    list(APPEND ASCENT_TPL_DLL_PATHS ${RAJA_DIR_ORIG}/lib/)
endif()