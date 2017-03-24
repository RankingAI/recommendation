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
		Redis(const std::string host_ = "127.0.0.1",const int port_ = 6379){
		  cpp_redis::active_logger = std::unique_ptr<cpp_redis::logger>(new cpp_redis::logger);
		  handle.connect(host_, port_, [](cpp_redis::redis_client& rp) {
			  std::cout << "client disconnected (disconnection handler)" << std::endl;
		  });
		  handle.sync_commit();
		}

		static boost::optional<Redis*> GetInstance(const std::string host_ = "127.0.0.1",const int port_ = 6379){
		  try{
		  	cpp_redis::redis_client tmp;
		  	tmp.connect(host_,port_,[](cpp_redis::redis_client&){});
			tmp.disconnect();
			return new Redis(host_,port_);
		  }
		  catch (std::exception const& e){
			return boost::none;
		  }
		}

		template <typename T1,typename T2>
		void HMultGetAll(std::vector<std::vector<std::pair<T1,T2>>>& result_,
						 const int& db_,const std::vector<std::string>& keys_){

		  	// select db
			handle.select(db_,[&db_](cpp_redis::reply& rp){
				//std::cout << "select db " << db_<< " result " << rp << std::endl;
			});
			auto iter_container = result_.begin() + 1;
			for(std::vector<std::string>::const_iterator iter_key = keys_.begin();
				iter_key != keys_.end() && iter_container != result_.end();
				iter_key++,iter_container++){
			  // ! all operations within rediscpp is asynchronous, if you want to submit comands in pipeline mode,
			  // you must pass the entire container you've already prepared into the command
				this->hgetall_((*iter_container),(*iter_key));
			}
			handle.sync_commit();

			// sort afterwards
			for(auto it = result_.begin() + 1;it != result_.end();it++){
				std::sort((*it).begin(), (*it).end(),
					[](const std::pair<T1, T2> &a, const std::pair<T1, T2> &b) {
							return std::get<1>(a) > std::get<1>(b);
					}
			  	);
		  	}
		};

		// ! function template, declare and definition CANNOT be seperated
		// field type T1, value type T2
		template <typename T1,typename T2>
		void HGetAll(std::vector<std::pair<T1,T2>>& result_,const int& db_,const std::string& key_){
		  // select
		  handle.select(db_,[&db_](cpp_redis::reply& rp){
			  //std::cout << "select db " << db_<< " result " << rp << std::endl;
		  });
		  // hgetall
		  // reply is a array, size will end up with 0 when broadcasterId does not exist
		  std::vector<std::pair<T1,T2>> result;
		  this->hgetall_(result,key_);
		  handle.sync_commit();
		  // ! don not check the size within operation, leave it to upper caller
		  //if(result.size() > 0) {
			// inner-sort by descent
			std::sort(result.begin(), result.end(),
					  [](const std::pair<T1, T2> &a, const std::pair<T1, T2> &b) {
						  return std::get<1>(a) > std::get<1>(b);
					  }
			);
			// copy into result
			result_.swap(result);
		  //}
		};

		// disconncect
		void release(){
		  handle.disconnect();
		}

	private:
		cpp_redis::redis_client handle;

		// hgetall command with template
		template <typename T1,typename T2>
		void hgetall_(std::vector<std::pair<T1,T2>>& result_,const std::string& key_){

		  handle.hgetall(key_,[&result_](cpp_redis::reply& rp){
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
				  result_.push_back(std::make_pair(keys[i],values[i]));
				}
			  }
			  else{
				std::cout << "Invalid hgetall result!" << std::endl;
			  }
		  });
		}
};

#endif //RECOMMENDATION_REDIS_H
