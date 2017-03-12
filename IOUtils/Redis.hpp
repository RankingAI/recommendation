/*
 * Created by yuan pingzhou on 3/10/17.
 */

#ifndef RECOMMENDATION_REDIS_H
#define RECOMMENDATION_REDIS_H

#include "cpp_redis/redis_client.hpp"

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>

class Redis {
	public:
		//
		Redis(const std::string host_ = "127.0.0.1",const int port_ = 6379):host(host_),port(port_){
		  cpp_redis::active_logger = std::unique_ptr<cpp_redis::logger>(new cpp_redis::logger);
		  handle.connect(host, port, [](cpp_redis::redis_client& rp) {
			  std::cout << "client disconnected (disconnection handler)" << std::endl;
		  });
		}

		// ! function template, declare and definition CANNOT be seperated
		// field type T1, value type T2
		template <typename T1,typename T2>
		void HGetAll(std::vector<std::pair<T1,T2>>& result_,const int& db_,const std::string& key_){
		  // select
		  handle.select(db_,[&db_](cpp_redis::reply& rp){
			  std::cout << "select db " << db_<< " result " << rp << std::endl;
		  });
		  // hgetall
		  // reply is a array, size will end up with 0 when broadcasterId does not exist
		  std::vector<std::pair<T1,T2>> result;
		  handle.hgetall(key_,[&result](cpp_redis::reply& rp){
			  std::vector<cpp_redis::reply> ReplyVec = rp.as_array();
			  if((ReplyVec.size() % 2) == 0) {
				int idx = 0;
				int PairSize = ReplyVec.size()/2;
				std::vector<T1> keys(PairSize);
				std::vector<T2> values(PairSize);
				for (std::vector<cpp_redis::reply>::iterator it = ReplyVec.begin(); it != ReplyVec.end(); it++) {
				  if((idx % 2) == 0){
					keys[idx/2] = boost::lexical_cast<T1>((*it).as_string());
				  }
				  else{
					values[idx/2] = boost::lexical_cast<T2>((*it).as_string());
				  }
				  idx++;
				}
				for(int i = 0;i < PairSize;i++){
				  result.push_back(std::make_pair(keys[i],values[i]));
				}
			  }
			  else{
				std::cout << "Invalid hgetall result!" << std::endl;
			  }
		  });
		  handle.sync_commit();
		  // inner-sort by descent
		  std::sort(result.begin(), result.end(),
					[=](std::pair<T1, T2>& a, std::pair<T1, T2>& b){
						return a.second > b.second;
					}
		  );
		  // copy into result
		  result_.swap(result);
		};
		// disconncect
		void release(){
		  handle.disconnect();
		}

	private:
		std::string host = "";
		int port = -1;
		cpp_redis::redis_client handle;
};

#endif //RECOMMENDATION_REDIS_H
