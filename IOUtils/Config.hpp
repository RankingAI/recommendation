/*
 * Created by yuan pingzhou on 3/16/17.
 */

#ifndef RECOMMENDATION_CONFIG_HPP
#define RECOMMENDATION_CONFIG_HPP

#include <boost/program_options.hpp>
#include <fstream>

namespace opt = boost::program_options;

class Config{
public:
	static Config& GetInstance(){
	  static Config instance;
	  return instance;
	}
	void Init(const std::string InputFile);
	template <typename T>
	T GetValue(const char* key_){
	  return vm[key_].as<T>();
	}

private:
	Config(){}
	opt::variables_map vm;
};

void Config::Init(const std::string InputFile){
  std::cout << "parse config file begins ." << std::endl;
  opt::options_description desc("All options");
  desc.add_options()
		  ("THRIFT_SERVER_PORT", opt::value<int>()->default_value(5001),
		   "thrift server port")
		  ("REDIS_SERVER_HOST", opt::value<std::string>()->default_value("10.0.8.81"), "Redis Host")
		  ("REDIS_SERVER_PORT", opt::value<int>()->default_value(6379), "Redis Port")
		  ("REDIS_SERVER_DB", opt::value<int>()->default_value(10), "Redis Port")
		  ("REDIS_TIMEOUT", opt::value<int>()->default_value(10000), "Redis Connect Timeout")
		  ("SIMILARITY_PREFIX", opt::value<std::string>()->default_value("BORADCASTER_SIMILARITY:"), "Similarity Prefix")
		  ("ENABLE_FREQUENCY_PREFIX", opt::value<std::string>()->default_value("BROADCASTER_ENABLE_FREQUENCY:"), "Enable Frequency Prefix")
	  ;
  // parse file
  try{
	std::ifstream InputStream(InputFile.c_str());
	opt::store(opt::parse_config_file(InputStream,desc,true),vm);
	notify(vm);
  }
  catch (const opt::reading_file& e){
	std::cerr << "parse config file failed ." << e.what() << std::endl;
  }
}

#endif //RECOMMENDATION_CONFIG_HPP
