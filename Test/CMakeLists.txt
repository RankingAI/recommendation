cmake_minimum_required(VERSION 3.5)

# generate executable target
ADD_EXECUTABLE(Test Test.cpp)
# boost IOUtils libs, -l
TARGET_LINK_LIBRARIES(Test boost_timer boost_system IOUtils)
