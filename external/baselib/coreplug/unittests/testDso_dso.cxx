#include <testdso_dso_export.h>

extern "C" {
	int TESTDSO_DSO_EXPORT mysym = 2342;
}

