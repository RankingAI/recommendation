CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

# ranking service include dir
SET(RANKINGSERVICE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
# export ranking service include dir to parent directory
SET(RANKINGSERVICE_INCLUDE_DIR ${RANKINGSERVICE_INCLUDE_DIR} PARENT_SCOPE)
# thrift source files
SET(THRIFT_GENERATED_SRCS ../RankingServiceProtocol/OnlineRankingService.cpp ../RankingServiceProtocol/RankingService_constants.cpp ../RankingServiceProtocol/RankingService_types.cpp)
# local source files
SET(RANKINGSERVICE_SRCS RankingService.hpp)

# ========================== build
IF(TARGET RankingService)
  MESSAGE("RankingService has already defined")
ELSE()
  # thrift include directory, -I
  INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/RankingServiceProtocol")
  # generate static library, this is NOT a HEADER-ONLY case
  ADD_LIBRARY(RankingService ${RANKINGSERVICE_SRCS} ${THRIFT_GENERATED_SRCS})
  # thrift IOUtils libs, -l
  TARGET_LINK_LIBRARIES(RankingService thrift IOUtils jsoncpp)
ENDIF()

# ============================ install
#install (TARGETS RankingService DESTINATION lib)
#install (FILES RankingService.hpp DESTINATION include)