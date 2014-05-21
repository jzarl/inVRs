#include "Steerable.h"

ufo::Steerable::~Steerable()
{
}

void ufo::Steerable::destroy()
{
	delete this;
}
