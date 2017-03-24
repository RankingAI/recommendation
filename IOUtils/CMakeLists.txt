cmake_minimum_required(VERSION 3.5)

# set IOUtils include directory
#SET(IOUTILS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
# export to parent directory, for the convenience of external references
#SET(IOUTILS_INCLUDE_DIR ${IOUTILS_INCLUDE_DIR} PARENT_SCOPE)
#
SET(REDIS_LIBS hiredis)
SET(IOUTILS_SRCS RedisClient.hpp Config.hpp)

# ===================== build
if(TARGET IOUtils)
  MESSAGE("IOUtils has already been defined")
else()
  # local include directory, -L
  #INCLUDE_DIRECTORIES(${IOUTILS_INCLUDE_DIR})
  # redis include directory, -L
  #INCLUDE_DIRECTORIES(/usr/local/include/cpp_redis /usr/local/include/tacopie)
  INCLUDE_DIRECTORIES(/usr/local/include/hiredis)
  # -o libioutils.la *.hpp
  ADD_LIBRARY(IOUtils INTERFACE)
  # redis libs, -l
  TARGET_LINK_LIBRARIES(IOUtils INTERFACE ${REDIS_LIBS})
  #
  TARGET_INCLUDE_DIRECTORIES(IOUtils INTERFACE ${IOUTILS_INCLUDE_DIR})
  # set linker language as CXX, hpp cannot be recognized by cmake
  #SET_TARGET_PROPERTIES(IOUtils PROPERTIES LINKER_LANGUAGE CXX)
endif()

# ====================== install
#install (TARGETS IOUtils DESTINATION lib)
#install (FILES IOUtils.hpp DESTINATION include)
