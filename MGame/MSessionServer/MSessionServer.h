#include "../MGame/MBaseServer.h"
#include "../MGame/MBaseSession.h"
#include <map>
#include <vector>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>

#define sServer SingleTen()

class MClientSessionManager
{
public:


	typedef std::map<size_t, MSessionAuthority *> MapSession;
	void DealPackage();	// visit all sessions and DealPackage()
	bool CheckAndAddSession(size_t index, MSessionAuthority *session);
	bool CheckAndDelSession(MSessionAuthority *session);
	void CheckAndDelSession(std::vector<MSessionAuthority *> &vS);
	bool CheckAndDelSession(size_t sessionIndex);

	// ��session�첽 �Ͽ��ǣ� �����������
	void OnNotifySessionClose(MSessionAuthority *session);

private:
	MapSession mSessions;
};


class MSessionAcceptor
{
public:
	MSessionAcceptor(boost::asio::io_service &ios, short port);

	void AsyncAccept();
	void OnAcceptEnd(MBaseSocket *sock, const boost::system::error_code& error);
private:
	boost::asio::ip::tcp::acceptor myAcceptor;
	boost::asio::io_service mIos;


};

class MSessionServer: public MBaseServer
{
public:
	MSessionServer(boost::asio::io_service &ios, short port);
	~MSessionServer();

	bool BootUp();

	// �����µ�socket
	virtual void HandleNewSocket() override;

	// ɾ���ɵ�socket
	virtual void HandleDelSocket() override;

	// �����յ���package
	virtual void DealPackage() override;

	// ����ʱ���йصĶ���
	virtual void UpdateTime() override;

	void WantToAddSession(MSessionAuthority *sa);

	void WantToDelSession(MSessionAuthority *sa);

	// ��session�첽 �Ͽ��ǣ� �����������
	void OnNotifySessionClose(MSessionAuthority *session);
protected:

	
private:
	struct BootUpConfig 
	{
		BootUpConfig(boost::asio::io_service &ios, short port)
			:mRefIos(ios), port(port){}

		boost::asio::io_service &mRefIos;
		short port;
	};
	BootUpConfig mBootUpConfig;
	MSessionAcceptor mAcceptor;
	// session that are created recently, should be added to system at next stamp
	std::vector<MSessionAuthority *> mSessionsToAdd;
	boost::mutex mMutexSessionAdd;

	// session that are deleted recently, should be deleted to system at next stamp
	std::vector<MSessionAuthority *> mSessionsToDel;
	boost::mutex mMutexSessionDel;

	MClientSessionManager *mClientSessionManager;
	MSessionAuthority *mSuperSession;


	
};

MSessionServer &SingleTen();
