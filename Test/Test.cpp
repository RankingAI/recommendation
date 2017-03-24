
/*
 * Created by yuan pingzhou on 3/10/17.
 */
//

#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/exception/all.hpp>
#include <boost/program_options.hpp>
#include <boost/variant.hpp>

#include "./ConcurrencyTest.hpp"
#include "./Singleton.hpp"
#include "./RedisTest.hpp"

namespace opt = boost::program_options;

int main(int argc,char* argv[]){

  if(argc < 2){
    std::cout << format("Usage: %s command [Options] ...") % argv[0];
    return 1;
  }
  else{
	std::string cmd = argv[1];
	if(iequals(cmd,"Concurrency")){
	  // description for options
	  opt::options_description desc("All options:");
	  desc.add_options()
			  ("threshold,t",opt::value<int>()->default_value(10),"threshold value")
			  ("help,h","help message")
			  ;
	  // map container for options
	  opt::variables_map vm;
	  try {
		opt::store(opt::parse_command_line(argc - 1, &(argv[1]), desc), vm);
	  }
	  catch (const opt::required_option& e){
		std::cout << "Failed to read command line parameters : " << e.what() << std::endl;
	  }
	  // notify for help
	  opt::notify(vm);
	  if(vm.count("help")){
		std::cout << desc << std::endl;
		return 1;
	  }
	  // process
	  ConcurrencyTest con;
	  con.run(vm["threshold"].as<int>());
	}
	else if(iequals(cmd,"ObjectPtr")){
	  typedef boost::variant<boost::blank, int, const char*,std::string> my_var_t;
	  my_var_t var;
	  std::cout << var.which() << std::endl;
	  var = "Hello, dear reader";
	  std::cout << var.which() << std::endl;
	}
	else if(iequals(cmd,"Singleton")){
	  std::string InputFile = "/Users/yuanpingzhou/project/workspace/c++/recommendation/ranking_service.properties";
	  Singleton::GetInstance().Init(InputFile);
	  std::cout << Singleton::GetInstance().GetValue<std::string>("REDIS_SERVER_HOST")<< "\t"
				<< Singleton::GetInstance().GetValue<int>("REDIS_SERVER_PORT") << std::endl;
	}
	else if (iequals(cmd,"RedisTest")){
	  std::vector<std::thread> threads;
	  for(int i = 0;i < 1;i++){
		threads.push_back(std::thread(&request));
	  }

	  for(auto& th : threads){
		th.join();
	  }
	}
  }

  return 0;
}
