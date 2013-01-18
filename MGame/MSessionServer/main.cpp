#include "MSessionServer.h"
#include <boost/date_time.hpp>
#include <boost/thread.hpp>
#include <iostream>

static MSessionServer *__server = 0;
MSessionServer & SingleTen()
{
	return *__server;
}

#include "../MGame/ZLogger.h"
int main( int argc, char* argv[] )
{
	Zebra::logger = new zLogger("ztserver", "gameserver");
	Zebra::logger->setLevel("all");

	boost::asio::io_service ios;
	__server = new MSessionServer(ios, 9999);
	MSessionServer &server = *__server;

	if(!server.BootUp())
		return -1;
	boost::thread myThread(boost::BOOST_BIND(&boost::asio::io_service::run, &ios));
	boost::posix_time::ptime last = boost::posix_time::microsec_clock::universal_time();
	do 
	{
		server.MainLoop();
		boost::posix_time::ptime now;
		now = boost::posix_time::microsec_clock::universal_time();
		boost::posix_time::time_duration timeDuration = now - last;
		if(timeDuration.ticks() < 130000)
			boost::this_thread::sleep(boost::posix_time::millisec(100));
		last = now;
// 		std::cout << timeDuration.ticks() << std::endl;
	} while (1);
	myThread.join();
	return 0;
}