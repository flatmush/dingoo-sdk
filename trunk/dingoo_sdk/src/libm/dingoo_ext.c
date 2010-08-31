#include "math.h"
#include "fdlibm.h"

// FIXME check for too large values ?
long lrint(double x)
{
	return (long)rint(x);
}
