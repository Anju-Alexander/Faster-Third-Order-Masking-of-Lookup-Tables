
//Third-order lookup table based compression scheme.  Author Anju Alexander and Annapurna Valiveti.

/*The code for header files aes.h, aes_share.h, aes_htable_RP.h, share.h and souce code files 
aes.c, aes_share.c, aes_htable_RP.c and share.c are taken from the public github repository https://github.com/coron/htable. 
The code for unmasked PRESENT is from http://www.lightweightcrypto.org/implementations.php.
The code for bitsliced implementation was taken from the public repository https://github.com/annapurna-pvs/Higher-Order-LUT-PRG
Few methods from these files are customized according to the target architecture requirements.*/

// This program is free software; you can redistribute it and/or modify it
// under the terms of the  GPL-3.0 license


#include <stdio.h>
#include "AES/aes.h"
#include "BITSLICE/bitslice.h"
#include "PRESENT/present.h"
#include "Util/driver_functions.h"
#include "time.h"
#if TRNG==1
#include "MK64F12.h"
#endif

int main()
{

    int nt = 10; //Number of times to repeat experiments
    int shares = shares_N; // #Input shares. Set the parameter in common.h.
    int cipher =AES_THIRD; //Cipher can be AES_THIRD, PRESENT_THIRD, AES_RP, BITSLICE or AES_HIGHER_ORDER_INCREASING_SHARES
    int type=BASIC; //for AES_HO_I_S it can be BASIC or LRV
   
    double time[11]={0,0,0,0,0,0,0,0,0,0,0};// To hold offline and online execution clock cycle count
    double time_b[1]={0};
    int i,k,al;
	long sec,nsec;
    double temp=0.0;
    rand_in();
    if(cipher==AES_THIRD||cipher==AES_HIGHER_ORDER_INCREASING_SHARES)
    {
        byte keyex[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
		byte inex[16] = { 0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34 };
		//Expected result:{0x39,0x25,0x84,0x1d,0x02,0xdc,0x09,0xfb,0xdc,0x11,0x85,0x97,0x19,0x6a,0x0b,0x32};
       
       byte in1[16], in2[16], out1[16], out2[16];
	
		byte key1[16], key2[16];

		for (i = 0; i < 16; i++)
		{
			key1[i] = keyex[i];
			key2[i] = keyex[i];
		}
		
		for (i = 0; i < 16; i++)
		{
			in1[i] = inex[i];
			in2[i] = inex[i];

		}


		for (k = 0; k < 16; k++)
		{
			out1[k] = 0x0;
		}
		
		unsigned int begin1, end1, begin2, end2;
        run_aes(in1, out1, key1, nt); //unprotected AES implementation
		if(cipher==AES_THIRD)
        run_aes_shares_third(in2,out2,key2,shares,type,nt,time); //customised third-order code for AES-128

		if(cipher==AES_HIGHER_ORDER_INCREASING_SHARES)
		run_aes_higher_order_increasing_shares(in2,out2,key2,shares,type,nt,time); //Coron's Higher order increasing shares implementation at order=3 for AES-128

		if (compare_output(out1, out2, 16)) //verifying the output 
		{
			if(cipher ==AES_THIRD)
			printf("Successful execution of LUT-based AES using customized third order scheme \n");

			else if(cipher ==AES_HIGHER_ORDER_INCREASING_SHARES && type ==BASIC)
			printf("Successful execution of LUT-based AES using increasing shares higher order scheme \n");

			else if(cipher ==AES_HIGHER_ORDER_INCREASING_SHARES && type ==LRV)
			printf("Successful execution of LUT-based AES using increasing shares higher order scheme using LRV\n");

		    printf("(Milli seconds) Offline timings:%f \n",time[0]);
			printf("(Milli seconds) Online timings:%f \n",time[1]);   
		}
		else
		{
			printf("Unsuccessful execution :(, pls check...");
		
		}
    }
	if(cipher==PRESENT_THIRD)
	{	
	   byte keyex[] ={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
       byte inex[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};//{0x32,0x43,0xf6,0xa8,0x88,0x5a,0x30,0x8d};
       byte in1[8],in2[8],out1[8],out2[8];
       byte key1[10],key2[10];
	   al=0;
	   for(i=0;i<10;i++)
	   {
		   key1[i]=keyex[i];
           key2[i]=keyex[i];
		}
		for(i=0;i<8;i++)
        {
            in1[i]=inex[i];//rand()%256;
            in2[i]=inex[i];
		}
		for(k=0;k<8;k++)
        {
            out1[k]=0x0;
            out2[k]=0x0;
        }
		present(in1,out1,key1); //unprotected PRESENT Implementation
		run_present_shares_third(in2,out2,key2,shares,time,nt,type);//customised third-order code for PRESENT
		if(compare_output(out1,out2,8))//verifying the output 
        {
            printf("Successful execution of LUT-based PRESENT using customized third order scheme\n");
			printf("(Milli seconds) Offline timings:%f \n",time[0]);
			printf("(Milli seconds) Online timings:%f \n",time[1]); 
        }

        else
            {
                printf("Unsuccessful execution :(, pls check...");
            }
		
	}
	if(cipher==BITSLICE)
	{
		byte n=shares_N;
		int i,j,k;
		/***********Test Vectors***************/
		byte keyex[16]={0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};
		byte inex[16]={0x32,0x43,0xf6,0xa8,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34};
		byte outex[16]={0x39,0x25,0x84,0x1d,0x02,0xdc,0x09,0xfb,0xdc,0x11,0x85,0x97,0x19,0x6a,0x0b,0x32};
		byte inex1[16]={0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf,0x1};
		byte in[16],in1[16],in2[16],out[16],out1[16],out2[16];
		byte key[16],key1[16],key2[16];
		for(i=0;i<16;i++)
		{
			key[i]=keyex[i];
			key1[i]=keyex[i];
			key2[i]=keyex[i];
		}
		for(i=0;i<16;i++)
		{
			in[i]=inex[i];
			in1[i]=inex[i];
			in2[i]=inex[i];
		}	
		
		run_bitslice(in1,out1,key1,nt);//unprotected AES-BITLICED Implementation
		run_bitslice_shares(in2,out2,key2,nt,time_b); //32-bit bitsliced implementation with shares
		if (compare_output(out1, out2, 16))//verifying the output 
		{
			printf("Successful execution of LUT-based AES using BITSLICE \n");
			printf("(Milli seconds) Overall timings:%f \n",time_b[0]); 
		}
		else
		{
			printf("Unsuccessful execution :(, pls check...");
		
		}
		
	}
	if(cipher==AES_RP)
	{
		volatile double time_b1[1]={0};
		byte n=shares_N; //Second-order, n=3.
		/****************Test vectors********************/
		volatile unsigned int begin1=0, end1=0;
		double time_spent=0.0;
		#if TRNG==0
			struct timespec begin, end;
		#endif
		byte keyex[16]={0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};
		byte inex[16]={0x32,0x43,0xf6,0xa8,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34};
		byte outex[16]={0x39,0x25,0x84,0x1d,0x02,0xdc,0x09,0xfb,0xdc,0x11,0x85,0x97,0x19,0x6a,0x0b,0x32};

		byte in1[16], in2[16], out1[16], out2[16];
	
		byte key1[16], key2[16];
		for (i = 0; i < 16; i++)
			{
				key1[i] = keyex[i];
				key2[i] = keyex[i];
			}
			
			for (i = 0; i < 16; i++)
			{
			in1[i] = inex[i];
			in2[i] = inex[i];

		}
		for (k = 0; k < 16; k++)
		{
			out1[k] = 0x0;
		}
		byte in[16],out[16];
		byte key[16];
		

		for(i=0;i<16;i++) 
				key[i]=keyex[i];
		
		for(i=0;i<16;i++) 
				in[i]=inex[i];
		
		for(k=0;k<16;k++)
        out[k]=0x0;

		run_aes(in1,out1,key1,nt); //unprotected AES implementation
		
		
		#if TRNG==0
        	clock_gettime(CLOCK_REALTIME, &begin);
        #endif // TRNG
		 #if TRNG==1
            reset_systick();
            begin1 = SysTick->VAL; // Obtains the start time
   		 #endif // TRNG
		for(int i=0;i<nt;i++)
		run_aes_share_RP(in2,out2,key2,n,nt);//AES_RP scheme with shares
		
		#if TRNG==1
			end1 = SysTick->VAL; // Obtains the stop time
			time_spent = ((double) (begin1-end1))/nt; // Calculates the time taken
    	#endif // TRNG

		#if TRNG==0
			clock_gettime(CLOCK_REALTIME, &end);
			sec = end.tv_sec - begin.tv_sec;
			nsec = end.tv_nsec - begin.tv_nsec;
			temp = sec + nsec*1e-9;

			time_b[0] = temp*UNIT/nt;
        #endif // TRNG
		if (compare_output(out1, out2, 16)) //verifying the output
		{
			printf("Successful execution of AES using RP \n");
			printf("(Milli seconds) Overall timings:%f \n",time_b[0]); 
		}
		else
		{
			printf("Unsuccessful execution :(, pls check...");
		
		}
	}
    rand_dein();
    return 0;
}