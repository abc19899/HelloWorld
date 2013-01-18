#ifndef __MBaseSession_h__
#define __MBaseSession_h__
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <deque>
#include "../MGame/BaseMessage.h"

// 由Server驱动
class BaseMessage;
class BaseMessageReceiver;
class MessageQueue
{
public:
	MessageQueue();
	void Switch();
	void AddMessage(BaseMessage *msg);
	BaseMessage *GetMessage();
private:
	boost::mutex mSwithcMutex;
	std::deque<BaseMessage *> mDeque[2];
	size_t mCurrentWriteQueue;

	size_t GetCurrentWriteQueue() const{
		return mCurrentWriteQueue;
	}
	size_t GetCurrentReadQueue() const{
		return 1 - mCurrentWriteQueue;
	}
};

class MBaseSession
{
public:
	MBaseSession();
	~MBaseSession();

	
protected:
	
private:

};


class MBaseSocket;
// 带权限的session， 负责管理权限
class MSessionAuthority
{
public:
	MSessionAuthority(MBaseSocket *sock);
	~MSessionAuthority();

	enum EnumAuthor
	{
		NO_AUTHOR,
		AUTHOR_CLIENT,
		AUTHOR_SUPER_SERVER,

		AUTHRO_END
	};

	virtual void DealPackage();

	void AsyncReadHeader();
	void OnReadHeaderFinish(const boost::system::error_code &ec, size_t bytes_transferred);
	void AsyncReadBody();
	void OnReadBodyFinish(const boost::system::error_code &ec, size_t bytes_transferred);
	void OnAsyncReadError();

	bool Closed();
protected:
	
	EnumAuthor m_author;
	MessageQueue *mQueue;	
	MBaseSocket *mSock;
	// socket is already closed, and upon system should remove this session latter;
	bool mClosed;

	BaseMessageReceiver mReceiver;
	//mutable void *mManager;
};



#endif // __MBaseSession_h__
