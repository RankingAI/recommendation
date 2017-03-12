/**
 * Created by yuanpingzhou on 3/9/17
**/

#include <iostream>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/exception/all.hpp>
#include <boost/lexical_cast.hpp>

#include "RecommendationConfig.h"
#include "RankingService.hpp"

using namespace boost;

int main(int argc,char* argv[]) {

  if((argc != 2) || (std::strlen(argv[1]) == 0)){
	std::cout << format("%s Version %d.%d") % argv[0] % RECOMMENDATION_VERSION_MAJOR % RECOMMENDATION_VERSION_MINOR << std::endl;
	std::cout << format("Usage: %s ServicePort[int|AnyString]\n") % argv[0];
	return 1;
  }
  int port = -1;
  try{
	port = lexical_cast<int>(argv[1]);
  }
  catch (exception& e){
	std::cerr << diagnostic_information(e,"atoi error") << std::endl;
  }
  RankingService* rs = nullptr;
  if(port == -1) {
	rs = new RankingService();
  }
  else{
	rs = new RankingService(port);
  }
  rs->run();
  delete rs;

  return 0;
}