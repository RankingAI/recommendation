/*
 * Created by yuan pingzhou on 3/13/17.
 */

#ifndef RECOMMENDATION_CONCURRENCYTEST_HPP
#define RECOMMENDATION_CONCURRENCYTEST_HPP

#include <iostream>
#include <future>
#include <chrono>
#include <vector>
#include <thread>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost;

class ConcurrencyTest{
public:
	ConcurrencyTest(){};
	void run(int v){
	  std::vector<std::future<int>> result;
	  for(int i = 0;i < v;i++){
		result.push_back(std::async(square, i));
	  }
	  int ret = 0;
	  std::vector<std::string> tmp;
	  for(auto& e : result){
		int k = e.get();
		ret += k;
		tmp.push_back(lexical_cast<std::string>(k));
	  }
	  std::cout << algorithm::join(tmp,",") << std::endl;
	  std::cout << "main-thread : " << "Result :" << ret << std::endl;
	}
	~ConcurrencyTest(){};

private:
	static int square(int x){
	  //std::cout << " sub-thread : " << std::this_thread::get_id() << "\t" << x << std::endl;
	  std::this_thread::sleep_for(std::chrono::milliseconds(100));
	  std::cout << x << std::endl;
	  //std::cout << std::this_thread::get_id() << std::endl;
	  return x * x;
	}
};

#endif //RECOMMENDATION_CONCURRENCYTEST_HPP
