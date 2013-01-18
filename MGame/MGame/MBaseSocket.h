#ifndef __MBaseSocket_h__
#define __MBaseSocket_h__

#include <boost/asio.hpp>

class Package
{

};
class PackageDeque
{

};

// 自驱动， 最终实现为一个线程或者系统回调
class MBaseSocket
{
public:
	MBaseSocket(boost::asio::io_service &ios);
	~MBaseSocket();

	// 异步发
	void Send(const Package &pack) const;

// 	// 循环接受， 放到队列里面去
// 	void ReceiveToDeque();

	boost::asio::ip::tcp::socket &GetSock(){
		return mSock;
	}

	void Close()
	{
		mSock.close();
	}
	
protected:
	
private:
	PackageDeque *m_deque;	// at current , it's not used
	boost::asio::ip::tcp::socket mSock;
	
};


#endif // __MBaseSocket_h__


