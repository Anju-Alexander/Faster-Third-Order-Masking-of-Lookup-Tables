typedef unsigned char byte;

/*********This file has global constants and common functions********/

#include <time.h>

#define AES_THIRD 1 //These values are specific to our customized third order scheme
#define PRESENT_THIRD 2
#define BITSLICE 3
#define AES_HIGHER_ORDER_INCREASING_SHARES 4


#define BASIC 2
#define LRV 3 
#define PRG 4



#define TRNG 0 // Set value to 1 to use device-specific TRNG. Set to 0 to use AES-based PRG.
/***********Input parameters to define********/

#define shares_N 4 // #shares.

#define VARIANT IPRG //// Either Normal (NPRG)/ Increasing (IPRG) variants
/***********end of input parameters**********/

#define UNIT 1000000 //Time unit (Milli seconds)

#define SNI_RM 0 // SNI_RM can be for AES_THIRD or PRESENT_THIRD. set value to 1 for using SNI full refresh scheme. Set 0 for Third order specific SNI refresh mask.
extern int cipher; // Cipher can be either AES/ PRESENT/ BITSLICE

typedef unsigned int word;
void rand_in(void);
void rand_dein(void);
void gen_rand(byte *a,int n);
unsigned int gen_rand32(void);


double cal_time(clock_t stop, clock_t start);
void reset_systick(void);

int compare_output(byte *out1,byte *out2,byte size);


