/*  newfloat.h*/
/* Replacement for float.h and limits.h  These are machine dependent constants
     which the user should set*/
#define FLT_MAX  1.7014118219281863150e+38 /* max decimal value of a "float" */
#define FLT_MIN  (-FLT_MAX) /* min decimal value of a "float" */
#define DBL_EPSILON  2.2204460492503131e-16 /*(1+epsilon=1)*/
#define DBL_MAX 1.7976931348623157e+308 /* max decimal value of a "double"*/
#define DBL_MIN (-DBL_MAX)  /* min decimal value of a "double" */
#define SHRT_MIN     (-32768)  /* min decimal value of a "short" */
#define SHRT_MAX     32767  /* max decimal value of a "short" */
