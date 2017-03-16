cmake_minimum_required(VERSION 3.5)

# generate executable target
ADD_EXECUTABLE(Test Test.cpp ConcurrencyTest.hpp Singleton.hpp)
# boost IOUtils libs, -l
TARGET_LINK_LIBRARIES(Test boost_timer boost_system boost_program_options IOUtils)
