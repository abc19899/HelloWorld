#ifndef _BASE_MESSAGE_H_
#define _BASE_MESSAGE_H_
#include <vector>
#include <cassert>

typedef int int32;
typedef unsigned char uint8;

class BaseMessage;

class BaseMessageReceiver
{
public:
	BaseMessageReceiver(BaseMessage *msg);


	void AttachTarget(BaseMessage *msg){
		assert(this->msg == NULL);
		this->msg = msg;
		mHeaderDecoded = false;
	}

	BaseMessage *DetachTarget(){
		BaseMessage *ret = msg;
		msg = NULL;
		return ret;
	}

	std::vector<uint8> &GetHeaderBuffer(){
		assert(msg);
		return header;
	}

	bool DecodeHeader();

	// should be called after DecodeHeader()
	std::vector<uint8> &GetBody();
private:
	std::vector<uint8> header;
	BaseMessage *msg;
	bool mHeaderDecoded;
};

class BaseMessageWriter
{
public:
	BaseMessageWriter(const BaseMessage &msg);

	std::vector<uint8> &GetResult(){
		return mOutData;
	}
private:
	std::vector<uint8> mOutData;
};

class BaseMessage
{
public:
	friend BaseMessageReceiver;
	friend BaseMessageWriter;
	BaseMessage(){}
	~BaseMessage(){}


	uint8 GetCmd1() const{
		return mCmd1;
	}
	uint8 GetCmd2() const{
		return mCmd2;
	}

	const std::vector<uint8> &GetBody() const{
		return mBody;
	}
protected:
	
private:
	uint8 mCmd1;
	uint8 mCmd2;
	std::vector<uint8> mBody;
};



#endif // _BASE_MESSAGE_H_
