#include "MSessionServer.h"
#include "../MGame/MBaseSocket.h"
#include <boost/bind.hpp>


void MSessionServer::DealPackage()
{
	if(mClientSessionManager)
		mClientSessionManager->DealPackage();
	if(mSuperSession)
		mSuperSession->DealPackage();
}

MSessionServer::~MSessionServer()
{
	{
		boost::mutex::scoped_lock lock(mMutexSessionAdd);
		for(std::vector<MSessionAuthority *>::iterator it = mSessionsToAdd.begin();
			it != mSessionsToAdd.end(); ++ it)
		{
			delete *it;
			*it = 0;
		}
	}

	if(mClientSessionManager)
	{
		delete mClientSessionManager;
		mClientSessionManager = 0;
	}

	if(mSuperSession)
	{
		delete mSuperSession;
		mSuperSession = 0;
	}
}

bool MSessionServer::BootUp()
{
	// TODO:
	mAcceptor.AsyncAccept();
	return true;
}


MSessionServer::MSessionServer( boost::asio::io_service &ios, short port )
:mBootUpConfig(ios,port),
mAcceptor(ios, port),
mClientSessionManager(0),
mSuperSession(0){
	// TODO:
	mClientSessionManager = new MClientSessionManager();
}


static int MakeIndex(MSessionAuthority *session)
{
	// TODO
	static int index = 0;
	return index ++;
}
void MSessionServer::HandleNewSocket()
{
	// TODO:
	boost::mutex::scoped_lock lock(mMutexSessionAdd);
	for(std::vector<MSessionAuthority *>::iterator it = mSessionsToAdd.begin();
		it != mSessionsToAdd.end(); ++ it)
	{
		mClientSessionManager->CheckAndAddSession(MakeIndex(*it), *it);
	}
	mSessionsToAdd.clear();
}

void MSessionServer::UpdateTime()
{
	// TODO:
}

void MSessionServer::WantToAddSession( MSessionAuthority *sa )
{
	boost::mutex::scoped_lock lock(mMutexSessionAdd);
	mSessionsToAdd.push_back(sa);
}

void MSessionServer::HandleDelSocket()
{
	// TODO:
	boost::mutex::scoped_lock lock(mMutexSessionDel);
	for(std::vector<MSessionAuthority *>::iterator it = mSessionsToDel.begin();
		it != mSessionsToDel.end(); ++ it)
	{
		mClientSessionManager->CheckAndDelSession(*it);
	}
}

void MSessionServer::WantToDelSession( MSessionAuthority *sa )
{
	boost::mutex::scoped_lock lock(mMutexSessionDel);
	mSessionsToDel.push_back(sa);
}

#include <iostream>

void MClientSessionManager::DealPackage()
{
//	std::cout << "session num " << mSessions.size() << std::endl;
	for(MapSession::iterator it = mSessions.begin();
		it != mSessions.end(); ++ it)
	{
		it->second->DealPackage();
	}
}

bool MClientSessionManager::CheckAndAddSession( size_t index, MSessionAuthority *session )
{
	if(mSessions.find(index) != mSessions.end())
		return false;
	mSessions[index] = session;
	//session->mManager = this;
	return true;
}

bool MClientSessionManager::CheckAndDelSession( MSessionAuthority *session )
{
	MapSession::iterator it;
	for(it = mSessions.begin();
		it != mSessions.end(); ++ it)
	{
		if(it->second == session)
			break;
	}
	if(it == mSessions.end())
		return false;
	mSessions.erase(it);
	return true;
}

bool MClientSessionManager::CheckAndDelSession( size_t sessionIndex )
{
	if(mSessions.find(sessionIndex) == mSessions.end())
		return false;
	//mSessions[sessionIndex].mManager = NULL;
	mSessions.erase(sessionIndex);
	return true;
}

static bool IsEqual(MSessionAuthority *v1, MSessionAuthority *v2)
{
	return v1 == v2;
}

class EqualHelp
{
public:
	EqualHelp(MSessionAuthority *dest)
		:dest(dest){}
	bool operator()(MSessionAuthority *src)
	{
		return dest == src;
	}
private:
	MSessionAuthority *dest;
};
void MClientSessionManager::CheckAndDelSession( std::vector<MSessionAuthority *> &vS )
{
	for(MapSession::iterator it = mSessions.begin();
		it != mSessions.end();)
	{
		if(vS.empty())
			break;
		EqualHelp equalHelp(it->second);
		std::vector<MSessionAuthority *>::iterator it2 = find_if(vS.begin(), vS.end(), equalHelp);
		bool bDel = (it2 != vS.end());
		while(it2 != vS.end())
		{
			it2 = vS.erase(it2);
			it2 = find_if(it2, vS.end(), equalHelp);
		}
		if(bDel)
		{
			mSessions.erase(it ++);
		}
		else
			++ it;
	}

}

MSessionAcceptor::MSessionAcceptor( boost::asio::io_service &ios, short port )
:myAcceptor(ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{

}

void MSessionAcceptor::AsyncAccept()
{
	MBaseSocket *newSock = new MBaseSocket(myAcceptor.get_io_service());
	myAcceptor.async_accept(newSock->GetSock(), boost::BOOST_BIND(&MSessionAcceptor::OnAcceptEnd, this, newSock, boost::asio::placeholders::error));
}

void MSessionAcceptor::OnAcceptEnd( MBaseSocket *sock, const boost::system::error_code& error )
{
	// check authorrity
	MSessionAuthority *newSession = new MSessionAuthority(sock);
	sServer.WantToAddSession(newSession);
	newSession->AsyncReadHeader();
	AsyncAccept();
}

