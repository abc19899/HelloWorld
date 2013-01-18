#ifndef _COMMON_DEFS_H_
#define _COMMON_DEFS_H_


#include <vector>
#include <string>

struct Point
{
	Point(int x, int y):x(x), y(y){}
	size_t x;
	size_t y;
};
struct Rect
{
	int x;	// beg of x
	int y;
	int ex;	// end of x, thus x is [x, ex)
	int ey;
};

typedef unsigned char uint8;

double Distance(const Point &a, const Point &b);

#define RW_CLASS_MEMBER(x,y)\
private: x m##y;\
public: const x &Get##y() const{ return m##y; }\
	void Set##y(const x &other){ m##y = other;}]

#define R_CLASS_MEMBER(x,y)\
private: x m##y;\
public: const x &Get##y() const{ return m##y; }

#define RW_SIMPLE_MEMBER(x,y)\
private: x m##y;\
public: x Get##y() const{ return m##y; }\
	void Set##y(x other){ m##y = other;}

#define R_SIMPLE_MEMBER(x,y)\
private: x m##y;\
public: x Get##y() const{ return m##y; }

bool operator ==(const std::vector<uint8> &umsg, const std::string &str);
bool operator !=(const std::vector<uint8> &umsg, const std::string &str);

#endif // _COMMON_DEFS_H_
