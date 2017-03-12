cmake_minimum_required(VERSION 3.5)
PROJECT(Recommendation)

# c++ version
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fpic -DDEBUG")

# The version number.
SET(RECOMMENDATION_VERSION_MAJOR 1)
SET(RECOMMENDATION_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to the source code
# PROJECT_SOURCE_DIR is project home direcotry
# PROJECT_BINARY_DIR is project building home directory
CONFIGURE_FILE(
        "${PROJECT_SOURCE_DIR}/RecommendationConfig.h.in"
        "${PROJECT_BINARY_DIR}/RecommendationConfig.h"
)

# ================================ Build
# local source files
SET(RECOMMENDATION_SRCS Recommendation.cpp)
#SET(CMAKE_INSTALL_PREFIX /ypz/local)

# add build include directory, -I option
INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")

# IOUtils is low-level library, referenced by other modules
ADD_SUBDIRECTORY("./IOUtils")

# Test is a independent module, executable
ADD_SUBDIRECTORY("./Test")

# Ranking client is a independent module, executable
ADD_SUBDIRECTORY("./RankingClient")

# trigger CMakeLists.txt in subdirectory RankingService
ADD_SUBDIRECTORY("./RankingService" RankingService)

# add ranking service include files directory, -I option
INCLUDE_DIRECTORIES(${RANKINGSERVICE_INCLUDE_DIR})

# entry files
ADD_EXECUTABLE(Recommendation ${RECOMMENDATION_SRCS})

# libraries relied, -l option
TARGET_LINK_LIBRARIES(Recommendation RankingService)

# ============================== Test
ENABLE_TESTING()
#define a macro to simplify adding tests, then use it
macro (DO_TEST arg result)
  ADD_TEST(RecommendationTest${arg} Recommendation ${arg})
  SET_TESTS_PROPERTIES(RecommendationTest${arg}
          PROPERTIES PASS_REGULAR_EXPRESSION ${result})
endmacro (DO_TEST)
# do a bunch of result based tests
DO_TEST(25 "25 is 5")
DO_TEST(-25 "-25 is 0")

# ================================ Install
# add the install targets
INSTALL(TARGETS Recommendation DESTINATION bin)
INSTALL(FILES "${PROJECT_BINARY_DIR}/RecommendationConfig.h"
        DESTINATION include)
