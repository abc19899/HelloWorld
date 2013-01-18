#ifndef _M_SESSION_FROM_CLIENT_H_
#define _M_SESSION_FROM_CLIENT_H_


#include "../MGame/MBaseSession.h"


class SS_Role;

class SS_ClientSession: public MSessionAuthority
{
public:
	enum EnumPermission
	{
		EP_NO_ROLE,
		EP_HAVE_ROLE,
		EP_ENTER_GAME,


	};


	virtual void DealPackage() override;

	void HandleCreateRole(BaseMessage *msg);

private:
	SS_Role *mRole;
	EnumPermission mPermission;
};

#endif // _M_SESSION_FROM_CLIENT_H_

