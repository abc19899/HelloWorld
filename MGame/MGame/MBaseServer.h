#ifndef __MBaseServer_h__
#define __MBaseServer_h__

class PackageDeque;
// 单例， 管理其他所有东西的加载等
// main驱动
class MBaseServer
{
public:
	MBaseServer();
	~MBaseServer();

	void MainLoop();

	// 接受新的socket
	virtual void HandleNewSocket() = 0;

	// 删除旧的socket
	virtual void HandleDelSocket() = 0;

	// 处理收到的package
	virtual void DealPackage() = 0;

	// 更新时间有关的东西
	virtual void UpdateTime() = 0;
protected:
	
private:
	PackageDeque *m_packDeque;

};


#endif // __MBaseServer_h__
