cmake_minimum_required(VERSION 3.5)

# ranking service include dir
#SET(RANKINGCLIENT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
# export ranking service include dir to parent directory
#SET(RANKINGCLIENT_INCLUDE_DIR ${RANKINGSERVICE_INCLUDE_DIR} PARENT_SCOPE)
# thrift source files
SET(THRIFT_GENERATED_SRCS ../RankingServiceProtocol/OnlineRankingService.cpp ../RankingServiceProtocol/RankingService_constants.cpp ../RankingServiceProtocol/RankingService_types.cpp)
# local source files
SET(RANKINGCLIETN_SRCS RankingClient.cpp)

# thrift include directory, -I
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/RankingServiceProtocol")
# generate static library, this is NOT a HEADER-ONLY case
ADD_EXECUTABLE(RankingClient ${RANKINGCLIETN_SRCS} ${THRIFT_GENERATED_SRCS})
# boost thrift IOUtils libs, -l
TARGET_LINK_LIBRARIES(RankingClient boost_timer boost_system boost_program_options thrift IOUtils)

# ============================ install
#INSTALL(TARGETS RankingClient DESTINATION bin)
