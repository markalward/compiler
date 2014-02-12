
#include <vector>

class A
{
public:
	int x;
	virtual ~A() {};
};

class B : public A;


int main (int argc, char const* argv[])
{
	std::vector<A *> s;
	B *b;
	s.push_back(b);
	
	return 0;
}


class B : public A
{
public:
	int y;
};
