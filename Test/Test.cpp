
/*
 * Created by yuan pingzhou on 3/10/17.
 */
//

#include "utils.cpp"
#include <iostream>
#include "../IOUtils/Redis.hpp"

int main(int argc,char* argv[]){
//  std::vector<std::pair<std::string,double >> l;
//  l.push_back(std::make_pair("joe",25.7));
//  l.push_back(std::make_pair("jack",28.20));
//  l.push_back(std::make_pair("kate",19.5));
//
//  utils::sort(l);
//  std::vector<std::pair<std::string,double>>::iterator it;
//  for(it = l.begin();it != l.end();it++){
//    std::cout << (*it).first << "\t" << (*it).second << std::endl;
//  }
  Redis* hd = new Redis("localhost",6379);
  std::vector<std::pair<std::string,double>> NeighborBroadcasters;
  std::string id = "joe";
  hd->HGetAll(NeighborBroadcasters,10,id);

  return 0;
}
