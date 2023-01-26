#ifndef _ASSERT_H
#define _ASSERT_H


#define ASSERT(expr) \
    if (!(expr)) \
        aFailed(__FILE__, __LINE__)

void aFailed(char*, int);

#endif // _ASSERT_H