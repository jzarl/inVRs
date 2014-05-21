#include <memory>
int main(void)
{
	std::unique_ptr<int> p (new int);

	return (p.get()) ? 0 : 1;
}

