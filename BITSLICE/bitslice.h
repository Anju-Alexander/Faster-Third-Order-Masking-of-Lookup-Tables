#ifndef __bitslice_h__
#define __bitslice_h__
#include "../Util/common.h"
#include "shares.h"
void run_bitslice(byte in[16],byte out[16],byte key[16],int nt);
void run_bitslice_shares(byte in[16],byte out[16],byte key[16],int nt,double *time_b);
#endif