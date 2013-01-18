
#include "CommonDefs.h"
#include <cmath>

double Distance( const Point &a, const Point &b )
{
	return sqrt((double)((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)));
}


bool operator ==(const std::vector<uint8> &umsg, const std::string &str)
{
	if(umsg.size() != str.size())
		return false;
	for(size_t i = 0; i < umsg.size(); ++ i)
	{
		if(umsg[i] != (uint8)str[i])
			return false;
	}
	return true;
}

bool operator !=(const std::vector<uint8> &umsg, const std::string &str)
{
	return !(umsg == str);
}