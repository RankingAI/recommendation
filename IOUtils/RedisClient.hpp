/*
 * Created by yuan pingzhou on 3/23/17.
 */

#ifndef RECOMMENDATION_REDISCLIENT_HPP
#define RECOMMENDATION_REDISCLIENT_HPP

#include <hiredis/hiredis.h>
#include <sys/time.h>
#include <time.h>

#include <boost/format.hpp>

// not thread-safe for the moment
class RedisClient{

public:

	// constructor && destructor, ! default timeout 30ms(0.03s)
	RedisClient(const std::string host_ = "127.0.0.1",const int port_ = 6379,const int db_ = 10,const int timeout_ = 30000);
	~RedisClient(){};

	// get connect or command state
	bool GetState();

	// single request, hgetall
	template <typename T1,typename T2>
	void HGetAll(std::vector<std::pair<T1,T2>>& result_,const std::string& key_){

	  redisReply* reply = NULL;
	  std::string cmd = "HGETALL " + key_;
	  reply = (redisReply* )redisCommand(hd,cmd.c_str());
	  if(reply != NULL){
		if((reply->type == REDIS_REPLY_ARRAY) && (reply->elements % 2 == 0)){
		  GetMultFV(result_,reply);
		}// end of extracting data from reply
		else{
		  state = false;
		  std::cout << "Command [" << cmd << "], result: either not an array or odd number results." <<
					"__FILE__ " << __FILE__ << "__LINE__ " << __LINE__ << std::endl;
		}
		freeReplyObject(reply);
	  }// end of command
	  else{
		state = false;
		std::cout << "Command [" << cmd << "] error: " << hd->errstr <<
				  "__FILE__ " << __FILE__ << "__LINE__ " << __LINE__ << std::endl;
	  }
	};

	// multiple request with pipeline mode, hgetall
	template <typename T1,typename T2>
	void HMultGetAll(std::vector<std::vector<std::pair<T1,T2>>>& result_,const std::vector<std::string>& keys_){

	  int RequestSize = keys_.size();
	  std::vector<redisReply*> replies;
	  replies.resize(RequestSize);
	  result_.resize(RequestSize);
	  for(int i = 0;i < RequestSize;i++){
		std::string cmd = "HGETALL " + keys_[i];
		redisAppendCommand(hd,cmd.c_str());
	  }
	  int cnt = 0;
	  for(int i = 0;i < RequestSize;i++){
		int ret = redisGetReply(hd,(void**)(&replies[i]));
		if(ret == REDIS_OK){
		  GetMultFV(result_[i],replies[i]);
		  cnt += 1;
		}
		if(replies[i] != NULL) {
		  freeReplyObject(replies[i]);
		}
		else{
		  std::cout << "Command HGETALL " << keys_[i] << " error: " << hd->errstr <<
					"__FILE__" << __FILE__ << "__LINE__" << __LINE__ << std::endl;
		}
	  }
	  if(cnt == RequestSize){
		state = true;
	  }
	  else{
		state = false;
	  }
	};

	// release connect handle
	void Release();

private:
	bool state = false;// result state for each command
	redisContext* hd = NULL;

	template <typename T1,typename T2>
	void GetMultFV(std::vector<std::pair<T1,T2>>& result_,redisReply* reply_){

	  int PairSize = (reply_->elements)/2;
	  std::vector<T1> keys;
	  std::vector<T2> values;
	  for(int i = 0;i < reply_->elements;i++){
		int FieldType = ((redisReply*)(reply_->element[i]))->type;
		switch(FieldType){
		  case REDIS_REPLY_INTEGER:
			if(i % 2 == 0) {
			  keys.push_back(boost::lexical_cast<T1>(reply_->element[i]->integer));
			}
			else{
			  values.push_back(boost::lexical_cast<T2>(reply_->element[i]->integer));
			}
			break;
		  case REDIS_REPLY_STRING:
			if(i % 2 == 0) {
			  keys.push_back(boost::lexical_cast<T1>(reply_->element[i]->str));
			}
			else{
			  values.push_back(boost::lexical_cast<T2>(reply_->element[i]->str));
			}
			break;
		  default:
			std::cout << "Exception type: " << FieldType << "__FILE__ " << __FILE__
					  << "__LINE__ " << __LINE__ << std::endl;
			break;
		}
	  }// end of for loop getting fields and values
	  for(int i = 0;i < PairSize;i++){
		result_.push_back(std::make_pair(keys[i],values[i]));
	  }
	  // inner-sort by descent
	  std::sort(result_.begin(), result_.end(),
				[](const std::pair<T1, T2> &a, const std::pair<T1, T2> &b) {
					return std::get<1>(a) > std::get<1>(b);
				}
	  );
	  state = true;
	};
};

RedisClient::RedisClient(const std::string host_, const int port_, const int db_, const int timeout_) {
  // connect with timeout
  struct timeval tval;
  tval.tv_sec = 0;
  tval.tv_usec = timeout_;
  hd = redisConnectWithTimeout(host_.c_str(),port_,tval);
  if(hd == NULL){
	state = false;
	std::cout << "Connection error: can't allocate redis context." << "__FILE__ "
			  << __FILE__ << "__LINE__ " << __LINE__ << std::endl;
  }
  else if(hd->err > 0){
	state = false;
	std::cout << "Connection error: " << hd->errstr << "__FILE__ " << __FILE__
			  << "__LINE__ " << __LINE__ << std::endl;
  }
  else{
	// select db
	/* Switch to DB 9 for testing, now that we know we can chat. */
	redisReply *reply = NULL;
	std::string cmd = "SELECT " + std::to_string(db_);
	reply = (redisReply*)redisCommand(hd,cmd.c_str());
	if(reply && boost::iequals(reply->str,"OK")){
	  state = true;
	  //std::cout << "Command [" << cmd << "] result: " << reply->str << std::endl;
	  freeReplyObject(reply);
	}
	else if(reply != NULL){
	  state = false;
	  std::cout << "Command [" << cmd  << "] error: " << reply->str << "__FILE__ "
				<< __FILE__ << "__LINE__ " << __LINE__ << std::endl;
	  freeReplyObject(reply);
	}
	else{
	  state = false;
	  std::cout << "Command [" << cmd << "] error: " << hd->errstr << "__FILE__ "
				<< __FILE__ << "__LINE__ " << __LINE__ << std::endl;
	}
  }
}

bool RedisClient::GetState(){
  return state;
}

void RedisClient::Release() {
  if(hd) {
	redisFree(hd);
	hd = NULL;
  }
}
#endif //RECOMMENDATION_REDISCLIENT_HPP
