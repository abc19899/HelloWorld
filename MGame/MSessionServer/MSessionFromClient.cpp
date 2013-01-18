#include "MSessionFromClient.h"
#include "../MGame/ZLogger.h"
#include "SS_Role.h"
#include "../MGame/CommonDefs.h"

enum EnumPara
{
	EP_CREATE_ROLE
};



struct Method
{
	typedef void (SS_ClientSession::*Func)(BaseMessage *);
	EnumPara para;
	SS_ClientSession::EnumPermission permission;
	Func func;
};

static const Method sMethodTable[] = 
{
	{EP_CREATE_ROLE, SS_ClientSession::EP_NO_ROLE, &SS_ClientSession::HandleCreateRole}
//	{EP_LOGIN, &SS_ClientSession::DealPackage}
};

static int FindFunc(uint8 cmd1)
{
	for(size_t i = 0; i < sizeof(sMethodTable) / sizeof(Method); ++ i)
		if(cmd1 == sMethodTable[i].para)
			return i;
	return -1;
}

void SS_ClientSession::DealPackage()
{
	mQueue->Switch();
	BaseMessage *msg = mQueue->GetMessage();
	while(msg)
	{
		try
		{
			int methodIndex = FindFunc(msg->GetCmd1());
			if(methodIndex >= 0)
			{
				if(sMethodTable[methodIndex].permission > mPermission)
					ZLOG_WARN("current permission(%d) < needPermision(%d)", mPermission, sMethodTable[methodIndex].permission);
				else
					(this->*sMethodTable[methodIndex].func)(msg);
			}
		}
		catch(std::runtime_error &)
		{
			ZLOG_WARN("op fail.");
		}
		delete msg;
		msg = mQueue->GetMessage();
	}
}



void SS_ClientSession::HandleCreateRole( BaseMessage *msg )
{
	if(msg->GetBody() != std::string("i want to create role"))
	{
		ZLOG_WARN("create role body fail");
		return;
	}
	mRole = new SS_Role();
	mPermission = EP_HAVE_ROLE;
}

