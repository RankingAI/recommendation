
/*
 * Created by yuan pingzhou on 3/10/17.
 */

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

class utils{
public:
	template <typename T1,typename T2>
	static void sort(std::vector<std::pair<T1,T2>>& l){
	  std::sort(l.begin(), l.end(), [=](std::pair<T1, T2>& a, std::pair<T1, T2>& b)
		   {
			   return a.second < b.second;
		   }
	  );
	}
};

