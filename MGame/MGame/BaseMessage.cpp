#include "BaseMessage.h"

BaseMessageReceiver::BaseMessageReceiver( BaseMessage *msg )
:header(sizeof(int) + sizeof(uint8) + sizeof(uint8)),
msg(msg){

}

#include "Zlogger.h"
bool BaseMessageReceiver::DecodeHeader()
{
	assert(msg);
	int size = *(int *)(&header[0]);
	if(size < 2)
		return false;
	msg->mCmd1 = header[4];
	if(msg->mCmd1 != 133)
	{
		ZLOG_ERROR("not 133");
		ZLOG_ERROR("%2.2x %2.2x %2.2x %2.2x %2.2x %2.2x ",
			header[0], header[1], header[2], header[3], header[4], header[5]);
		return false;
	}
	msg->mCmd2 = header[5];
	msg->mBody.resize(size - 2);
	mHeaderDecoded = true;
	return true;
}

std::vector<uint8> &BaseMessageReceiver::GetBody()
{
	assert(msg);
	assert(mHeaderDecoded);
	return msg->mBody;
}


BaseMessageWriter::BaseMessageWriter( const BaseMessage &msg )
{
	mOutData.resize(sizeof(int32) + sizeof(msg.mCmd1) + sizeof(msg.mCmd2) + msg.mBody.size());
	*(int32 *)(&mOutData[0]) = mOutData.size() - sizeof(int32);
	mOutData[4] = msg.mCmd1;
	mOutData[5] = msg.mCmd2;
	memcpy(&mOutData[6], &msg.mBody[0], msg.mBody.size());
}
