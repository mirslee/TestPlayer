#include "stdafx.h"
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "MxFixups.h"

int strncasecmp (const char *s1, const char *s2, size_t n)			// sunqueen modify
{
#ifdef HAVE_STRNICMP
    return strnicmp (s1, s2, n);			// sunqueen modify
#else
    for (size_t i = 0; i < n; i++)
    {
        unsigned char c1 = s1[i], c2 = s2[i];
        int d = tolower (c1) - tolower (c2);
        if (d || !c1)
            return d;
        assert (c2);
    }
    return 0;
#endif
}
