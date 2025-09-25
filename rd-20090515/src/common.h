// common.h

#ifndef COMMON_H
#define COMMON_H

#include <math.h>

// M_PI isn't guaranteed by C99 on all platforms
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#endif // COMMON_H
