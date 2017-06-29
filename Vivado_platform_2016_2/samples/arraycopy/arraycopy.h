#ifndef _ZYBO_LED_TEST2_H
#define _ZYBO_LED_TEST2_H
#define N 16
#define NUMBYTES 4*N

typedef unsigned int uint32;

#pragma SDS data copy(a[0:N])
#pragma SDS data copy(b[0:N])
void arraycopy(uint32 *a, uint32 *b);
void arraycopy_sw(uint32 *a, uint32 *b);
#endif
