namespace cpp ranking

service OnlineRankingService{
	string ranking(1: string broadcasterId,2: string adList);
}
