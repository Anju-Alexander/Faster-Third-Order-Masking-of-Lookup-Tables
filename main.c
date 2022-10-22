
//Third-order lookup table based compression scheme.

#if TRNG  ==1
#include "MK64F12.h"
#endif
#include <stdio.h>

#include "AES/aes.h"
#include "BITSLICE/bitslice.h"
#include "PRESENT/present.h"
#include "Util/driver_functions.h"
#include "time.h"

int main()
{

    int nt = 10; //Number of times to repeat experiments
    int shares = shares_N; // #Input shares. Set the parameter in common.h.
    int cipher = AES_RP; //Cipher can be AES_THIRD, PRESENT_THIRD, AES_RP, BITSLICE or AES_HIGHER_ORDER_INCREASING_SHARES
    int type=BASIC; //for AES_THIRD type can be BASIC, for PRESENT_THIRD type can be BASIC,  AES_HO_I_S it can be BASIC or LRV
   
    double time[11]={0,0,0,0,0,0,0,0,0,0,0};// To hold offline and online execution clock cycle count
    double time_b[1]={0};
    int i,k,al;
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
        run_aes(in1, out1, key1, nt);
		if(cipher==AES_THIRD)
        run_aes_shares_third(in2,out2,key2,shares,type,nt,time);

		if(cipher==AES_HIGHER_ORDER_INCREASING_SHARES)
		run_aes_higher_order_increasing_shares(in2,out2,key2,shares,type,nt,time);

		if (compare_output(out1, out2, 16))
		{
			if(cipher ==AES_THIRD)
			printf("Successful execution of LUT-based AES using customized third order scheme \n");

			else if(cipher ==AES_HIGHER_ORDER_INCREASING_SHARES && type ==BASIC)
			printf("Successful execution of LUT-based AES using increasing shares higher order scheme \n");

			else if(cipher ==AES_HIGHER_ORDER_INCREASING_SHARES && type ==LRV)
			printf("Successful execution of LUT-based AES using increasing shares higher order scheme using LRV\n");

		    printf("Offline timings:%f \n",time[0]);
			printf("Online timings:%f \n",time[1]/nt);   
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
		present(in1,out1,key1);
		run_present_shares_third(in2,out2,key2,shares,time,nt,type);
		if(compare_output(out1,out2,8))
        {
            printf("Successful execution of LUT-based PRESENT using customized third order scheme\n");
			printf("Offline timings:%f \n",time[0]);
			printf("Online timings:%f \n",time[1]/nt); 
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
		
		run_bitslice(in1,out1,key1,nt);
		run_bitslice_shares(in2,out2,key2,nt,time_b);
		if (compare_output(out1, out2, 16))
		{
			printf("Successful execution of LUT-based AES using BITSLICE \n");
			printf("Overall timings:%f \n",time_b[0]/nt); 
		}
		else
		{
			printf("Unsuccessful execution :(, pls check...");
		
		}
		
	}
	if(cipher==AES_RP)
	{
		volatile double time_b[1]={0};
		byte n=4,l=1; //Second-order, n=3.
		/****************Test vectors********************/
		volatile unsigned int begin1=0, end1=0;
		double time_spent=0.0;
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
		//printf("Inside main ...Compression!!!\n");

		for(i=0;i<16;i++) 
				key[i]=keyex[i];
		
		for(i=0;i<16;i++) 
				in[i]=inex[i];
		
		for(k=0;k<16;k++)
        out[k]=0x0;

		run_aes(in1,out1,key1,nt);

		#if TRNG==1
            reset_systick();
            begin1 = SysTick->VAL; // Obtains the start time
   		#endif // TRNG
		#if TRNG==0
        	clock_t begin = clock(); // Obtains the start time
   		#endif // TRNG
		for(int i=0;i<nt;i++)
		run_aes_share_RP(in2,out2,key2,n,nt);//AES with shares

		#if TRNG==1
			end1 = SysTick->VAL; // Obtains the stop time
			time_b[0] = ((double) (begin1-end1))/nt; // Calculates the time taken
			#endif // TRNG
		#if TRNG==0
			clock_t end = clock();
			time_spent =+ (double)(end - begin)/CLOCKS_PER_SEC;
			time_b[0] = time_spent;
    	#endif // TRNG 
		if (compare_output(out1, out2, 16))
		{
			printf("Successful execution of AES using RP \n");
			printf("Overall timings:%f \n",time_b[0]/nt); 
		}
		else
		{
			printf("Unsuccessful execution :(, pls check...");
		
		}
	}
    rand_dein();
    return 0;
}