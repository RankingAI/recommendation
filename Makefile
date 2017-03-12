BOOST_DIR = /usr/include/boost
THRIFT_DIR = /usr/local/include/thrift
LIB_DIR = /usr/local/lib
#GEN_SRC = ./gen-cpp/user_types.cpp ./gen-cpp/user_constants.cpp ./gen-cpp/UserService.cpp
GEN_SRC = ./RankingServiceProtocol/OnlineRankingService.cpp ./RankingServiceProtocol/RankingService_constants.cpp ./RankingServiceProtocol/RankingService_types.cpp
RANKING_SERVICE_DIR = ./RankingService
GEN_DIR = ./RankingServiceProtocol
IOUTILS_DIR = ./IOUtils

all:recommendation

recommendation:Recommendation.cpp ${RANKING_SERVICE_DIR}/RankingService.cpp
	g++ -g -o recommendation -I${THRIFT_DIR} -I${BOOST_DIR} -I. -I${GEN_DIR} -I${RANKING_SERVICE_DIR} -L${LIB_DIR} -lboost_system -lboost_timer -lthrift -ltacopie -lcpp_redis Recommendation.cpp ${RANKING_SERVICE_DIR}/RankingService.cpp ${IOUTILS_DIR}/Redis.cpp ${GEN_SRC}

clean:
	rm *.o recommendation
