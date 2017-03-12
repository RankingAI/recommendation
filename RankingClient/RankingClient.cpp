/**
 * Created by yuanpingzhou on 3/9/17
 */

#include <iostream>

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

#include "../RankingServiceProtocol/OnlineRankingService.h"

using namespace boost;
using namespace apache::thrift;
using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;
using namespace ranking;

typedef boost::chrono::duration<double> sec; // seconds, stored with a double

class RankingClient {

public:
	// connect with server
	RankingClient(const std::string& host_ = "127.0.0.1",const int port_ = 5001):ServerHost(host_),ServerPort(port_){
	  boost::shared_ptr<TTransport> socket_(new TSocket(ServerHost, ServerPort));
	  transport = boost::make_shared<TBufferedTransport>(socket_);
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
		transport = nullptr;
	  }
	  catch (TException& x){
		std::cerr << diagnostic_information(x,"ranking error") << std::endl;
	  }
	  return result;
	}
	//
	~RankingClient(){
	  delete handle;
	  handle = nullptr;
	}

private:
	std::string ServerHost = "";
	int ServerPort = -1;
	OnlineRankingServiceClient* handle = nullptr;
	boost::shared_ptr<TTransport> transport = nullptr;
};

int main(int argc,char* argv[]){
	// client
	if((argc != 6) || (std::strlen(argv[2]) == 0) || (std::strlen(argv[3]) == 0)){
	  std::cout << "Please check your parameters. " << std::endl;
	  std::cout << format("Usage: %s RankingClient ServiceHost ServicePort BroadcasterID AdList\n") % argv[0];
	  return 1;
	}

	std::string host = argv[2];
	int port = -1;
	std::string bid = argv[4];
	std::string InitialAdList = argv[5];
	try{
	  port = lexical_cast<int>(argv[3]);
	}
	catch (exception& e){
	  std::cerr << diagnostic_information(e,"atoi error") << std::endl;
	}

	boost::timer::cpu_timer timer;
	RankingClient* rc = nullptr;
	if(host.empty() || (port == -1)) {
	  rc = new RankingClient();
	}
	else {
	  rc = new RankingClient(host, port);
	}
	std::cout << format("InitialAdList : %s \n") % InitialAdList;
	std::string RankedAdList = rc->ranking(bid,InitialAdList);
	sec seconds = boost::chrono::nanoseconds(timer.elapsed().user);
	std::cout << format("RankedAdList : %s, Time elaspsed: %f \n") % RankedAdList % seconds.count();
	delete rc;

	return 0;
}
