/**
 * Created by yuan pingzhou on 3/8/17.
**/

#ifndef RECOMMENDATION_RANKINGSERVICE_H
#define RECOMMENDATION_RANKINGSERVICE_H

// thrift
#include <thrift/server/TNonblockingServer.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PlatformThreadFactory.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>
#include <json/json.h>
#include <json/value.h>

// boost
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/container/set.hpp>

// customized
#include "../RankingServiceProtocol/OnlineRankingService.h"
#include <../IOUtils/RedisClient.hpp>
#include "../IOUtils/Config.hpp"

// standard
#include <iostream>
#include <map>

using namespace boost;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace apache::thrift::concurrency;

using namespace ranking;

std::string GetNeighbors(const std::vector<std::pair<std::string,double>>& vec){
  std::vector<std::string> f;
  // ! a normal iterator is not permitted by c++11, either auto and const_iterator is good option
  //for(std::vector<std::pair<std::string,double>>::const_iterator it = vec.begin();it != vec.end();it++){...}
  for(auto it = vec.begin();it != vec.end();it++){
	f.push_back((*it).first);
  }
  return algorithm::join(f,",");
}

std::string GetFeatures(const std::vector<std::pair<std::string,int>>& vec){
  std::vector<std::string> f;
  // ! a normal iterator is not permitted by c++11
  for(std::vector<std::pair<std::string,int>>::const_iterator it = vec.begin();it != vec.end();it++){
	f.push_back((*it).first);
  }
  return algorithm::join(f,",");
}

#define REQUESTOUTPUT(bid,rq) {std::cout << format("Request: broadcaster id %s request list %s\n") % bid % rq;}
#define NEIGHBOROUTPUT(bid,nl) {std::cout << format("Broadcaster %s's neighbours %s\n") % bid % GetNeighbors(nl);}
#define FEATUREOUTPUT(bid,fl) {std::cout << format("Broadcaster %s's features %s\n") % bid % GetFeatures(fl);}
#define RESULTOUTPUT(bid,rs) {std::cout << format("Result: broadcaster id %s result list %s\n") % bid % rs;}

/*
 * ranking kernel
 */
class OnlineRankingServiceHandler : virtual public OnlineRankingServiceIf {
public:

	void ranking(std::string& _return, const std::string& BroadcasterId, const std::string& RequestList) {

		#ifdef DEBUG
			REQUESTOUTPUT(BroadcasterId,RequestList);
		#endif
	  	Json::Value ResultListJsonContainer;
	  	std::set<std::string> RequestSet;
	  	Json::Value RequestListJsonContainer;
	  	Json::Reader JsonReader;
		auto ret = JsonReader.parse(RequestList, RequestListJsonContainer);
	 	if(ret == false) {
		  std::cerr << "Parse json request failed." << std::endl;
		}
	  	for(auto iter = RequestListJsonContainer.begin();iter != RequestListJsonContainer.end();iter++){
		  RequestSet.insert((*iter).asString());
		}

	  	int RequestSize = RequestSet.size();

	  	// validate for Request List whose elements should not be duplicated
	  	std::vector<std::vector<std::pair<std::string, int>>> AllFeatures;
	  	if(RequestListJsonContainer.size() == RequestSize){
		  RedisClient hd(Config::GetInstance()->GetValue<std::string>("REDIS_SERVER_HOST"),
						 Config::GetInstance()->GetValue<int>("REDIS_SERVER_PORT"),
						 Config::GetInstance()->GetValue<int>("REDIS_SERVER_DB"),
						 Config::GetInstance()->GetValue<int>("REDIS_TIMEOUT")
		  );
		  if(hd.GetState()) { // state of connection
			// get neighbour broadcasters
			std::vector<std::pair<std::string, double>> NeighborBroadcasters;
			hd.HGetAll(NeighborBroadcasters,
					   Config::GetInstance()->GetValue<std::string>("SIMILARITY_PREFIX") + BroadcasterId);
			if (hd.GetState()) { // state of getting neighbour broadcasters
#ifdef DEBUG
			  NEIGHBOROUTPUT(BroadcasterId,NeighborBroadcasters);
#endif
			  //  initialize feature container
			  AllFeatures.resize(NeighborBroadcasters.size() + 1);
			  // get neighbour broadcasters' features
			  if (NeighborBroadcasters.size() > 0) {
				std::vector<std::string> nbs;
				nbs.push_back(Config::GetInstance()->GetValue<std::string>("ENABLE_FREQUENCY_PREFIX") + BroadcasterId);
				for (auto it = NeighborBroadcasters.begin(); it != NeighborBroadcasters.end(); it++) {
				  nbs.push_back(Config::GetInstance()->GetValue<std::string>("ENABLE_FREQUENCY_PREFIX") + (*it).first);
				}
				hd.HMultGetAll(AllFeatures, nbs);
				if (hd.GetState() == false) {
				  std::cout << "Failed to get neighbour features." << std::endl;
				}
			  }// end of if NeighborBroadcasters.size() > 0
			}
			else {
			  std::cout << "Failed to get neighbours." << std::endl;
			}
		  }
		  else{
			std::cout << "Failed to establish redis handle." << std::endl;
		  }
		  hd.Release();

			// construct result feature list
		  std::vector<std::vector<std::pair<std::string, int>>>::iterator it;
		  for (it = AllFeatures.begin(); it != AllFeatures.end(); it++) {
		    std::vector<std::pair<std::string, int>>::iterator it1;
			for (it1 = (*it).begin(); it1 != (*it).end(); it1++) {
			  std::string feature = (*it1).first;
			  if (RequestSet.find(feature) != RequestSet.end()) {
			    ResultListJsonContainer.append(Json::Value(feature));
			    RequestSet.erase(feature);
			  }
			}
		  } // end of for

		  // append the remained features into result list
		  if ((RequestSet.size() > 0) && (RequestSet.size() < RequestSize)) {
		    std::set<std::string>::iterator it;
		    for (it = RequestSet.begin(); it != RequestSet.end(); it++) {
		      ResultListJsonContainer.append(Json::Value((*it)));
			}
		  } // end of if
		} // end of if TmpList.size() == RequestSet.size()

		// validate the lenght of result ad list
		if(ResultListJsonContainer.size() == RequestSize){
			Json::StyledWriter styledWriter;
			_return = styledWriter.write(ResultListJsonContainer);
		}
	  	else{
			_return = RequestList;
	  	}
		#ifdef DEBUG
			RESULTOUTPUT(BroadcasterId,_return);
		#endif
	}
};

/*
 * handle factory for ranking service, reuse handles, otherwise all connections will end up with sharing the same handle
 */
class OnlineRankingServiceHandlerFactory: virtual public OnlineRankingServiceIfFactory{
public:
	virtual ~OnlineRankingServiceHandlerFactory(){}
	virtual OnlineRankingServiceIf* getHandler(const :: apache::thrift::TConnectionInfo& connInfo){
	  boost::shared_ptr<TSocket> sock = boost::dynamic_pointer_cast<TSocket>(connInfo.transport);
	  std::cout << "Incoming connection \n";
	  std::cout << format("\t SocketInfo: %s ") % sock->getSocketInfo();
	  std::cout << format("\t PeerHost: %s ") % sock->getPeerHost();
	  std::cout << format("\t PeerAddress: %s ") % sock->getPeerAddress();
	  std::cout << format("\t PeerPort: %s \n") % sock->getPeerPort();
	 //
	  OnlineRankingServiceHandlerFactory::RequestCount+= 1;
	  std::cout << OnlineRankingServiceHandlerFactory::RequestCount << std::endl;
	  return new OnlineRankingServiceHandler();
	}
	virtual void releaseHandler(OnlineRankingServiceIf* handler) {
		delete handler;
	  	handler = nullptr;
	}

private:
	static int RequestCount;
};

int OnlineRankingServiceHandlerFactory::RequestCount = 0;

/*
 * ranking service entry
 */
class RankingService {
private:
	boost::shared_ptr<TNonblockingServer> serverPtr = nullptr;
	const int WorkerCount = 100;

public:
	RankingService(int ServerPort_){

	  // Thread manager, reuse of threads
	  boost::shared_ptr<ThreadManager> ThreadManager_ = ThreadManager::newSimpleThreadManager(WorkerCount);
	  ThreadManager_->threadFactory(boost::make_shared<PlatformThreadFactory>());
	  ThreadManager_->start();

	  // Server-side level: multi-thread non-block IO mode
	  serverPtr = boost::make_shared<TNonblockingServer>(
			  //boost::make_shared<OnlineRankingServiceProcessorFactory>(boost::make_shared<OnlineRankingServiceHandlerFactory>()),
			  boost::make_shared<OnlineRankingServiceProcessor>(boost::make_shared<OnlineRankingServiceHandler>()),
			  boost::make_shared<TBinaryProtocolFactory>(),
			  ServerPort_,
			  ThreadManager_
	  );
	}

	static boost::optional<RankingService*> GetInstance(const std::string& ConfigFile){

		int rp = -1;
		try {
		  Config::GetInstance()->Init(ConfigFile.c_str());
		  rp = Config::GetInstance()->GetValue<int>("THRIFT_SERVER_PORT");
		  return new RankingService(rp);
		}
		catch (const exception& e){
		  return boost::none;
		}
	}

	~RankingService(){
	  serverPtr->stop();
	  serverPtr = nullptr;
	}

	void run(){
	  serverPtr->serve();
	}
};

#endif //RECOMMENDATION_RANKINGSERVICE_H
