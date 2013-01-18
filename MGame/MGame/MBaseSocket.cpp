#include "MBaseSocket.h"


MBaseSocket::~MBaseSocket()
{
	if(m_deque)
	{
		delete m_deque;
		m_deque = 0;
	}
}



MBaseSocket::MBaseSocket( boost::asio::io_service &ios )
:m_deque(0),
mSock(ios){

}
