#include "../AES/aes.h"

/************specific to aes third order************ */
 void run_aes_shares_third(byte *in, byte *out, byte *key, int n, int type, int nt, double time[11]);
 /**************specific to present third orde***************/
 void run_present_shares_third(byte *in,byte*out,byte *key,int n,double *time,int nt, int type);
 /************specific to higher order increasing shares**************/
 void run_aes_higher_order_increasing_shares(byte *in, byte *out, byte *key, int n, int type, int nt, double time[11]);
 /*******************AES_RP********************************************/
 void run_aes_share_RP(byte in[16],byte out[16],byte key[16],int n,int nt,double *time_b);
 //void run_bitslice_shares(byte in[16],byte out[16],byte key[16],int nt,double *time_b)
