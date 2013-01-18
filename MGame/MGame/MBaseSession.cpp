#include "MBaseSession.h"
#include "../MGame/BaseMessage.h"
#include "../MGame/MBaseSocket.h"
#include <boost/bind.hpp>

#include "../MSessionServer/MSessionServer.h"


MSessionAuthority::~MSessionAuthority()
{
	// TODO:
}


MSessionAuthority::MSessionAuthority( MBaseSocket *sock )
:mQueue(new MessageQueue()),
mSock(sock),
mClosed(false),
mReceiver(NULL){
	// TODO:
}


#include "ZLogger.h"

void MSessionAuthority::DealPackage()
{
	mQueue->Switch();
	BaseMessage *msg = mQueue->GetMessage();
	while(msg)
	{
		std::cout << "cmd1(" << int(msg->GetCmd1())
			<< ") cmd2(" << int(msg->GetCmd2())
			<< ") data(";
		for(size_t i = 0; i < msg->GetBody().size(); ++ i)
			std::cout << (int)(unsigned char)msg->GetBody()[i] << ' ';
		std::cout << ")" << std::endl;
		delete msg;
		msg = mQueue->GetMessage();
	}
}

void MSessionAuthority::AsyncReadHeader()
{
	BaseMessage *newMsg = new BaseMessage();
	mReceiver.AttachTarget(newMsg);
	boost::asio::async_read(this->mSock->GetSock(),
		boost::asio::buffer(mReceiver.GetHeaderBuffer()),
		boost::BOOST_BIND(&MSessionAuthority::OnReadHeaderFinish, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void MSessionAuthority::AsyncReadBody()
{
	boost::asio::async_read(this->mSock->GetSock(),
		boost::asio::buffer(mReceiver.GetBody()),
		boost::BOOST_BIND(&MSessionAuthority::OnReadBodyFinish, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	
}

static void LogMessage(boost::asio::ip::tcp::socket &sock)
{

	boost::system::error_code ec;
	//boost::asio::ip::tcp::socket
	std::vector<boost::asio::mutable_buffer > buffs;
	buffs.resize(1);
	buffs[0] = boost::asio::mutable_buffer(new char[1000000], 1000000);
	// 		buffs[1] = boost::asio::mutable_buffer(new char[1000], 1000);
	// 		std::vector<boost::asio::basic_streambuf<std::allocator<char> > > streamBufferVec(1);

	size_t readLen = sock.read_some(buffs, ec);
	const void *data = boost::asio::detail::buffer_cast_helper(buffs[0]);
	size_t dataLen = boost::asio::detail::buffer_size_helper(buffs[0]);

	ZLOG_DEBUG16("body:", (const BYTE *)data, readLen);



	//boost::asio::mutable_buffer
	//		std::vector<char> logData(1000);
	//		ZLOG_DEBUG16("header:", _umessage->data(), _umessage->header_first_length);
	//close();
}

void MSessionAuthority::OnReadHeaderFinish( const boost::system::error_code &ec, size_t bytes_transferred )
{
	if(ec)
	{
		OnAsyncReadError();
		return;
	}

	if(!mReceiver.DecodeHeader())
	{
		LogMessage(mSock->GetSock());
		OnAsyncReadError();
		return;
	}
	AsyncReadBody();
}


void MSessionAuthority::OnReadBodyFinish( const boost::system::error_code &ec, size_t bytes_transferred )
{
	if(ec)
	{
		OnAsyncReadError();
		return;
	}


	this->mQueue->AddMessage(mReceiver.DetachTarget());

	AsyncReadHeader();
}


void MSessionAuthority::OnAsyncReadError()
{
	BaseMessage *msg = mReceiver.DetachTarget();
	delete msg;
	mSock->Close();
	mClosed = true;
	sServer.WantToDelSession(this);
}


void MessageQueue::Switch()
{
	boost::mutex::scoped_lock lock(mSwithcMutex);
	mCurrentWriteQueue = 1 - mCurrentWriteQueue;
}

MessageQueue::MessageQueue()
:mCurrentWriteQueue(0){

}

void MessageQueue::AddMessage( BaseMessage *msg )
{
	boost::mutex::scoped_lock lock(mSwithcMutex);
	mDeque[GetCurrentWriteQueue()].push_back(msg);
}

BaseMessage * MessageQueue::GetMessage()
{
	boost::mutex::scoped_lock lock(mSwithcMutex);
	if(mDeque[GetCurrentReadQueue()].empty())
		return NULL;
	BaseMessage *ret = mDeque[GetCurrentReadQueue()].front();
	mDeque[GetCurrentReadQueue()].pop_front();
	return ret;
}
