/**
 * Created by yuanpingzhou on 3/9/17
 */

#include <iostream>
#include <thread>
#include <atomic>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <boost/exception/all.hpp>
#include <boost/make_shared.hpp>
#include <boost/chrono.hpp>
#include <boost/timer/timer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>

#include "../RankingServiceProtocol/OnlineRankingService.h"

using namespace boost;
using namespace apache::thrift;
using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;
using namespace ranking;

typedef boost::chrono::duration<double> sec; // seconds, stored with a double
std::atomic<int> success(0);

namespace opt = boost::program_options;

class RankingClient {

public:
	// connect with server
	RankingClient(const std::string& host_ = "127.0.0.1",const int port_ = 5001):ServerHost(host_),ServerPort(port_){

	  boost::shared_ptr<TSocket> socket_(new TSocket(host_, port_));
	  socket_->setConnTimeout(1000);
	  socket_->setRecvTimeout(1000);
	  socket_->setSendTimeout(1000);
	  transport = boost::make_shared<TFramedTransport>(socket_);
	  boost::shared_ptr<TProtocol> protocol_(new TBinaryProtocol(transport));
	  handle = new ranking::OnlineRankingServiceClient(protocol_);
	}
	// ranking
	std::string ranking(std::string BroadcasterID,std::string AdList){
	  std::string result;
	  try {
		transport->open();
		handle->ranking(result,BroadcasterID,AdList);
		transport->close();
		success += 1;
	  }
	  catch (TException& x){
		std::cerr << x.what() << std::endl;
	  }
	  return result;
	}
	//
	~RankingClient(){
	  if(handle != nullptr) {
		delete handle;
		handle = nullptr;
	  }
	}

private:
	std::string ServerHost = "";
	int ServerPort = -1;
	OnlineRankingServiceClient* handle = nullptr;
	boost::shared_ptr<TTransport> transport = nullptr;
};

void Ranking(const std::string& host,const int& port,const std::string& bid,const std::string& RequestList){

  std::string RankedAdList = "";
  if(host.empty() || (port == -1)) {
	RankingClient rc;
	RankedAdList = rc.ranking(bid,RequestList);
  }
  else {
	RankingClient rc(host, port);
	RankedAdList = rc.ranking(bid,RequestList);
  }
  std::cout << RankedAdList << std::endl;
}

int main(int argc,char* argv[]){
  	std::string RequestString = "[\"20161130180008224220970\",\"20170210165405973705135\",\"20170106214719565544151\" \
			,\"20161228150040169553309\",\"20170104181929665240262\",\"20170112201142557644326\",\"20170110191853599794075\" \
			,\"20170104205841052933424\",\"20161219183300419813605\",\"20170106215025161822143\",\"20161222220344051672602\" \
			,\"20170214142351157914891\",\"20161130175727184878123\",\"20161130175921301130830\"]";

	opt::options_description desc("options");
  	desc.add_options()
			("ServerHost",opt::value<std::string>()->default_value("localhost"),"Ranking Service Host")
			("ServerPort",opt::value<int>()->default_value(5001),"Ranking Service Port")
			("bid",opt::value<std::string>()->default_value("0e25d1dff0294c2f9bc71da9ca7060fd_204196826"),"Broadcaster ID")
			("RequestList",opt::value<std::string>()->default_value(RequestString),"Request List")
			("RequestNum",opt::value<int>()->default_value(100),"Request Number")
			("help,h","what to options to type in")
			;

  	opt::variables_map vm;
  	try{
		opt::store(opt::parse_command_line(argc,argv,desc),vm);
	  	opt::notify(vm);
  	}
  	catch(const opt::required_option& e){
		std::cerr << "Failed to parse parameters, " << e.what() << std::endl;
  	}

 	boost::timer::cpu_timer timer;

  	std::vector<std::thread> threads;
  	for(int i = 0;i < vm["RequestNum"].as<int>();i++){
	  threads.push_back(std::thread(&Ranking,vm["ServerHost"].as<std::string>(),
			  vm["ServerPort"].as<int>(),vm["bid"].as<std::string>(),vm["RequestList"].as<std::string>()));
	}

  	for(auto& th : threads){
	  th.join();
	}

	sec seconds = boost::chrono::nanoseconds(timer.elapsed().user);
	std::cout << format("RankedAdList : %s, Time elaspsed: %fnsec success %d \n") % vm["RequestList"].as<std::string>()
			 % (seconds.count()) % success.load(std::memory_order_relaxed);;

	return 0;
}
