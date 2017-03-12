# CMake generated Testfile for 
# Source directory: /Users/yuanpingzhou/project/workspace/c++/recommendation
# Build directory: /Users/yuanpingzhou/project/workspace/c++/recommendation/cmake-build-debug
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(RecommendationTest25 "Recommendation" "25")
set_tests_properties(RecommendationTest25 PROPERTIES  PASS_REGULAR_EXPRESSION "25 is 5")
add_test(RecommendationTest-25 "Recommendation" "-25")
set_tests_properties(RecommendationTest-25 PROPERTIES  PASS_REGULAR_EXPRESSION "-25 is 0")
subdirs(IOUtils)
subdirs(Test)
subdirs(RankingClient)
subdirs(RankingService)
