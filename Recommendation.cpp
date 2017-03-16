/**
 * Created by yuanpingzhou on 3/9/17
**/

#include <iostream>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/exception/all.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>

#include "RecommendationConfig.h"
#include "RankingService.hpp"

using namespace boost;
namespace opt = boost::program_options;

int main(int argc,char* argv[]) {

  opt::options_description desc("options : ");
  desc.add_options()
          ("config",opt::value<std::string>()->default_value("./ranking_service.properties"),"Ranking Service Configure File")
          ("help,h","what to options to type in")
          ;
  opt::variables_map vm;
  try{
    opt::store(opt::parse_command_line(argc,argv,desc),vm);
  }
  catch(const opt::required_option& e){
	std::cerr << "Failed to parse parameters, " << e.what() << std::endl;
  }
  boost::optional<RankingService> rs = RankingService::GetInstance(vm["config"].as<std::string>());
  if(rs){
	rs.get().run();
  }
  else{
    std::cout << "Initial ranking service failed, you'd better check your configure file thoroughly." << std::endl;
  }

  return 0;
}