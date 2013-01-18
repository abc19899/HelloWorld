#ifndef __MBaseSocket_h__
#define __MBaseSocket_h__

#include <boost/asio.hpp>

class Package
{

};
class PackageDeque
{

};

// �������� ����ʵ��Ϊһ���̻߳���ϵͳ�ص�
class MBaseSocket
{
public:
	MBaseSocket(boost::asio::io_service &ios);
	~MBaseSocket();

	// �첽��
	void Send(const Package &pack) const;

// 	// ѭ�����ܣ� �ŵ���������ȥ
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


