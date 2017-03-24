/*
 * Created by yuan pingzhou on 3/23/17.
 */
#include <../IOUtils/RedisClient.hpp>
#include <vector>

void request(){
  RedisClient RedisHandle;
  if(RedisHandle.GetState()){
	// todo
	std::cout << "Establish redis handle success." << std::endl;
	std::vector<std::vector<std::pair<std::string,int>>> result;
	//RedisHandle.HGetAll(result,"BORADCASTER_SIMILARITY:0e25d1dff0294c2f9bc71da9ca7060fd_204196826");
	//RedisHandle.HGetAll(result,"BROADCASTER_ENABLE_FREQUENCY:0e25d1dff0294c2f9bc71da9ca7060fd_2600612");
	//RedisHandle.HGetAll(result,"BORADCASTER_SIMILARITY:joe");
	//RedisHandle.HGetAll(result,"BROADCASTER_ENABLE_FREQUENCY:kate");
	std::vector<std::string> req;
	req.push_back("BROADCASTER_ENABLE_FREQUENCY:kate");
	req.push_back("BROADCASTER_ENABLE_FREQUENCY:jack");
	req.push_back("BROADCASTER_ENABLE_FREQUENCY:john");
	req.push_back("BROADCASTER_ENABLE_FREQUENCY:sawyer");
	RedisHandle.HMultGetAll(result,req);
	if(RedisHandle.GetState()){
	  std::cout << "HMultGetAll success." << std::endl;
	  int i = 0;
	  for(auto it = result.begin();it != result.end();it++) {
		std::cout << req[i] << std::endl;
		for (auto it1 = (*it).begin(); it1 != (*it).end(); it1++) {
		  std::cout << (*it1).first << "\t" << (*it1).second << std::endl;
		}
		i++;
	  }
	}
	else{
	  std::cout << "HGetAll failed." << std::endl;
	}
  }
  else{
	std::cout << "Failed to establish redis handle." << std::endl;
  }
  RedisHandle.Release();
}
