#ifndef __MBaseServer_h__
#define __MBaseServer_h__

class PackageDeque;
// ������ �����������ж����ļ��ص�
// main����
class MBaseServer
{
public:
	MBaseServer();
	~MBaseServer();

	void MainLoop();

	// �����µ�socket
	virtual void HandleNewSocket() = 0;

	// ɾ���ɵ�socket
	virtual void HandleDelSocket() = 0;

	// �����յ���package
	virtual void DealPackage() = 0;

	// ����ʱ���йصĶ���
	virtual void UpdateTime() = 0;
protected:
	
private:
	PackageDeque *m_packDeque;

};


#endif // __MBaseServer_h__
