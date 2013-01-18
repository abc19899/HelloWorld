#include "MBaseServer.h"


void MBaseServer::MainLoop()
{
	HandleDelSocket();

	HandleNewSocket();

	DealPackage();

	UpdateTime();
}

MBaseServer::MBaseServer()
{

}

MBaseServer::~MBaseServer()
{

}
