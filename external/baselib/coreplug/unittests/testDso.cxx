#include <coreplug/Dso.h>
#include <iostream>

int main(int argc, const char** argv)
{
	Dso *mydso = new Dso();
	if ( argc != 2 )
	{
		std::cerr << "Test case called with invalid number of arguments!" << std::endl;
		return 255;
	}
	// open self:
	if (!mydso->open(argv[1]))
	{
		std::cerr << "Could not open plugin file " << argv[1] <<std::endl;
		std::cerr << "Error: " << mydso->lastError() << std::endl;
		return 1;
	}
	// open succeeded
	if (!mydso->isValid())
	{
		std::cerr << "DSO is not valid after opening." << std::endl;
		std::cerr << "Error: " << mydso->lastError() << std::endl;
		return 1;
	}
	const int *dsosym = reinterpret_cast<const int*>(mydso->rawSymbol("mysym"));
	// mysym exists?
	if ( dsosym == 0 )
	{
		std::cerr << "Symbol not found." << std::cerr;
		std::cerr << "Error: " << mydso->lastError() << std::endl;
		return 1;
	}
	if ( *dsosym != 2342)
	{
		std::cerr << "Symbol value mismatch: got " << *dsosym << ", wanted 2342" << std::cerr;
		return 1;
	}
	std::cerr << "Success." << std::cerr;
	return 0;
}
