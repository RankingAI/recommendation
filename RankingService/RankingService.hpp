/**
 * Created by yuan pingzhou on 3/8/17.
**/

#ifndef RECOMMENDATION_RANKINGSERVICE_H
#define RECOMMENDATION_RANKINGSERVICE_H

// thrift
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PlatformThreadFactory.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>

// boost
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/container/set.hpp>
#include <boost/sort/spreadsort/string_sort.hpp>

// customized
#include "../RankingServiceProtocol/OnlineRankingService.h"
#include "../IOUtils/Redis.hpp"
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
	OnlineRankingServiceHandler() {}

	void ranking(std::string& _return, const std::string& BroadcasterId, const std::string& RequestList) {

		#ifdef DEBUG
			REQUESTOUTPUT(BroadcasterId,RequestList);
		#endif
	  // convert request list into a hash set
		std::vector<std::string> ResultList;
	  	std::vector<std::string> TmpList;
	  	boost::split(TmpList,RequestList,boost::is_any_of(","));
	  	std::set<std::string> RequestSet(TmpList.begin(),TmpList.end());
	  	int RequestSize = RequestSet.size();

	  // validate for Request List whose elements should not be duplicated
	  	if(TmpList.size() == RequestSet.size()) {

			// ----------------- Interact with redis
			// get all neighbour features
			std::vector<std::vector<std::pair<std::string, int>>> AllFeatures;
			Redis *hd = new Redis(RedisHost, RedisPort);
			// get features for its self
			std::vector<std::pair<std::string, int>> features;
			hd->HGetAll(features, RedisDB, "BROADCASTER_ENABLE_FREQUENCY:" + BroadcasterId);
			if (features.size() > 0) {
			  AllFeatures.push_back(features);
			}

			#ifdef DEBUG
				FEATUREOUTPUT(BroadcasterId,features);
			#endif

			// get neighbour broadcasters
			std::vector<std::pair<std::string, double>> NeighborBroadcasters;
			hd->HGetAll(NeighborBroadcasters, RedisDB, "BORADCASTER_SIMILARITY:" + BroadcasterId);

			#ifdef DEBUG
				NEIGHBOROUTPUT(BroadcasterId,NeighborBroadcasters);
			#endif

			// get neighbour features of broadcaster
			if (NeighborBroadcasters.size() > 0) {

			  for (std::vector<std::pair<std::string, double>>::iterator it = NeighborBroadcasters.begin();
				   it != NeighborBroadcasters.end(); it++) {
				std::string NeighborBroadcaster = (*it).first;
				std::vector<std::pair<std::string, int>> NeighborFeatures;
				hd->HGetAll(NeighborFeatures, RedisDB, "BROADCASTER_ENABLE_FREQUENCY:" + NeighborBroadcaster);

				#ifdef DEBUG
					FEATUREOUTPUT(NeighborBroadcaster,NeighborFeatures);
				#endif

				if(NeighborFeatures.size() > 0) {
				  AllFeatures.push_back(NeighborFeatures);
				}
			  } // end of for

			}// end of if NeighborBroadcasters.size() > 0
			hd->release();
			// -------------- end of interacting with redis

			// construct result feature list
			std::vector<std::vector<std::pair<std::string, int>>>::iterator it;
			for (it = AllFeatures.begin(); it != AllFeatures.end(); it++) {
			  std::vector<std::pair<std::string, int>>::iterator it1;
			  for (it1 = (*it).begin(); it1 != (*it).end(); it1++) {
				std::string feature = (*it1).first;
				if (RequestSet.find(feature) != RequestSet.end()) {
				  ResultList.push_back(feature);
				  RequestSet.erase(feature);
				}
			  }
			} // end of for

			// append the remained features into result list
			if ((RequestSet.size() > 0) && (RequestSet.size() < RequestSize)) {
			  std::set<std::string>::iterator it;
			  for (it = RequestSet.begin(); it != RequestSet.end(); it++) {
				ResultList.push_back((*it));
			  }
			} // end of if
		} // end of if TmpList.size() == RequestSet.size()

		// validate the lenght of result ad list
		if(ResultList.size() == RequestSize){
			_return = algorithm::join(ResultList,",");
	  	}
	  	else{
			_return = RequestList;
	  	}
		#ifdef DEBUG
			RESULTOUTPUT(BroadcasterId,_return);
		#endif
	}

private:
	std::string RedisHost = "127.0.0.1";
	int RedisPort = 6379;
	int RedisDB = 10;

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
	  return new OnlineRankingServiceHandler();
	}
	virtual void releaseHandler(OnlineRankingServiceIf* handler){
	  delete handler;
	}
};

/*
 * ranking service entry
 */
class RankingService {
private:
	TServer* serverPtr = nullptr;
	int ServicePort = -1;
	const int WorkerCount = 4;

public:
	RankingService(const int port_ = 5001):ServicePort(port_) {
	  // Thread manager, reuse of threads
	  boost::shared_ptr<ThreadManager> ThreadManager_ = ThreadManager::newSimpleThreadManager(WorkerCount);
	  ThreadManager_->threadFactory(boost::make_shared<PlatformThreadFactory>());
	  ThreadManager_->start();
	  // Server-side level: multi-thread non-block IO mode
	  serverPtr = new TThreadPoolServer(
	  	boost::make_shared<OnlineRankingServiceProcessorFactory>(boost::make_shared<OnlineRankingServiceHandlerFactory>()),
	  	boost::make_shared<TServerSocket>(ServicePort),
	  	boost::make_shared<TBufferedTransportFactory>(),
	  	boost::make_shared<TBinaryProtocolFactory>(),
		ThreadManager_
	  );
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
