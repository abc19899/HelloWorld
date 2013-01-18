#ifndef _COMONENT_H_
#define _COMONENT_H_

class Comonent
{
public:
	Comonent();
	~Comonent();

	virtual void AddChild(Comonent *);
	virtual bool CheckAndDelChild(Comonent *);
	
protected:
	
private:
	mutable Comonent *parent;
};

#endif // _COMONENT_H_