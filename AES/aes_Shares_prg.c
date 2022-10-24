#include <stdio.h>
#include <time.h>
#include "../Util/common.h"

#if TRNG == 1
#include "MK64F12.h"
#else
#include <time.h>
#endif


#include "../Util/share.h"
//#include "../Util/prg3.h"
#include "../Util/driver_functions.h"

#include "aes.h"
#include "aes_htable_prg.h"
#include "aes_htable_RP.h"

byte wshare[176][shares_N];


//*************************Code from Coron's github************

void keyexpansion_share(byte key[16],int n)
{
      byte w[176];
      keyexpansion(key,w);

      for(int i=0;i<176;i++)
      {
        share_rnga(w[i],wshare[i],n);
      }

}

void addroundkey_share(byte stateshare[16][shares_N],int round,int n)
{
      int i,j;
      for(i=0;i<16;i++)
        for(j=0;j<n;j++)
          stateshare[i][j]^=wshare[16*round+i][j];
}



void shiftrows_share(byte stateshare[16][shares_N],int n)
{
      byte m;
      int i;
      for(i=0;i<n;i++)
      {
        m=stateshare[1][i];
        stateshare[1][i]=stateshare[5][i];
        stateshare[5][i]=stateshare[9][i];
        stateshare[9][i]=stateshare[13][i];
        stateshare[13][i]=m;

        m=stateshare[2][i];
        stateshare[2][i]=stateshare[10][i];
        stateshare[10][i]=m;
        m=stateshare[6][i];
        stateshare[6][i]=stateshare[14][i];
        stateshare[14][i]=m;

        m=stateshare[3][i];
        stateshare[3][i]=stateshare[15][i];
        stateshare[15][i]=stateshare[11][i];
        stateshare[11][i]=stateshare[7][i];
        stateshare[7][i]=m;
      }
}



void mixcolumns_share(byte stateshare[16][shares_N],int n)
{
      byte ns[16];
      int i,j;
      for(i=0;i<n;i++)
      {
        for(j=0;j<4;j++)
        {
          ns[j*4]=multx(stateshare[j*4][i]) ^ multx(stateshare[j*4+1][i]) ^ stateshare[j*4+1][i] ^ stateshare[j*4+2][i] ^ stateshare[j*4+3][i];
          ns[j*4+1]=stateshare[j*4][i] ^ multx(stateshare[j*4+1][i]) ^ multx(stateshare[j*4+2][i]) ^ stateshare[j*4+2][i] ^ stateshare[j*4+3][i];
          ns[j*4+2]=stateshare[j*4][i] ^ stateshare[j*4+1][i] ^ multx(stateshare[j*4+2][i]) ^ multx(stateshare[j*4+3][i]) ^ stateshare[j*4+3][i];
          ns[j*4+3]=multx(stateshare[j*4][i]) ^ stateshare[j*4][i] ^ stateshare[j*4+1][i] ^ stateshare[j*4+2][i] ^ multx(stateshare[j*4+3][i]) ;
        }
        for(j=0;j<16;j++)
          stateshare[j][i]=ns[j];
      }
}

void aes_share_subkeys(byte in[16],byte out[16],int n,void (*subbyte_share_call)(byte *,int,int))
{
      int i;
      int round=0;

      byte stateshare[16][shares_N];

      for(i=0;i<16;i++)
      {
        share_rnga(in[i],stateshare[i],n);
      }

      addroundkey_share(stateshare,0,n);

      for(round=1;round<10;round++)
      {
        subbytestate_share_prg(stateshare,n,subbyte_share_call,round-1);
        shiftrows_share(stateshare,n);
        mixcolumns_share(stateshare,n);
        addroundkey_share(stateshare,round,n);
      }

      subbytestate_share_prg(stateshare,n,subbyte_share_call,round-1);
      shiftrows_share(stateshare,n);
      addroundkey_share(stateshare,10,n);

      for(i=0;i<16;i++)
      {
        out[i]=decode(stateshare[i],n);
        //free(stateshare[i]);
      }
}

/*************specific to third order***************/

 void aes_share_subkeys_third(byte in[16], byte out[16], int n, void(*subbyte_share_call)(byte *, int, int, int), int choice)
{
	int i, tmp = 0;
	int round = 0;  
	byte stateshare[16][shares_N];
	for (i = 0; i < 16; i++)
	{
		share_rnga(in[i], stateshare[i], n);
	}
	addroundkey_share(stateshare, 0, n); 
	for (round = 1; round < 10; round++)
	{
		subbytestate_share_third(stateshare, n, subbyte_share_call, round - 1, choice);
		
		shiftrows_share(stateshare, n);
		mixcolumns_share(stateshare, n);
		addroundkey_share(stateshare, round, n);
	}

	subbytestate_share_third(stateshare, n, subbyte_share_call, round - 1, choice);
	shiftrows_share(stateshare, n);
	addroundkey_share(stateshare, 10, n);

	for (i = 0; i < 16; i++)
	{
		out[i] = decode(stateshare[i], n);
		//free(stateshare[i]);
	}
}

void run_aes_third(byte in[16], byte out[16], byte key[16], int n, void(*subbyte_share_call)(byte *, int, int, int), int nt, int choice, double time[11])
{
	int i;
	keyexpansion_share(key, n);
	unsigned int begin1=0,end1=0;
  double time_spent = 0.0;
  long sec,nsec;
    double temp=0.0;
	#if TRNG==0
    struct timespec begin, end;
	
	#endif
  #if TRNG==0
        clock_gettime(CLOCK_REALTIME, &begin);
        #endif // TRNG
	for (i = 0; i < nt; i++)
	{
		#if TRNG==1
            reset_systick();
            begin1 = SysTick->VAL; // Obtains the start time
            #endif // TRNG
    
		aes_share_subkeys_third(in, out, n, subbyte_share_call, choice);
		
		#if TRNG==1
            end1 = SysTick->VAL; // Obtains the stop time
            time[i+1] = ((double) (begin1-end1)); // Calculates the time taken
            #endif // TRNG
      
   
	}
   #if TRNG==0
        clock_gettime(CLOCK_REALTIME, &end);
        sec = end.tv_sec - begin.tv_sec;
        nsec = end.tv_nsec - begin.tv_nsec;
        temp = sec + nsec*1e-9;

        time[1] = temp*UNIT/nt;
    #endif // TRNG 

}

void run_aes_higher_order_increasing_shares(byte *in, byte *out, byte *key, int n, int type, int nt, double time[11])
{
	
	unsigned int begin1, end1, begin2, end2;
	long sec, nsec;
	double temp = 0.0;
	double tim[1];
  double time_spent=0.0;
  #if TRNG==0
    struct timespec begin, end;
	
	#endif
  if(type == BASIC)
  {
    //inititialze the word sbox
    #if TRNG==1
      reset_systick();
      begin1 = SysTick->VAL; // Obtains the start time
    #endif // TRNG
    #if TRNG==0
        clock_gettime(CLOCK_REALTIME, &begin);
    #endif // TRNG
    gen_t_for_all_higher_increasing_shares(n,type,tim);
    #if TRNG==1
      end1 = SysTick->VAL; // Obtains the stop time
      time[0] = (double) (begin1-end1); // Calculates the time taken
      //time[0]=tim[0];
    #endif // TRNG
    #if TRNG==0
        clock_gettime(CLOCK_REALTIME, &end);
        sec = end.tv_sec - begin.tv_sec;
        nsec = end.tv_nsec - begin.tv_nsec;
        temp = sec + nsec*1e-9;

        time[0] = temp*UNIT/nt;
        #endif // TRNG
    
    run_aes_third(in, out, key, n, &subbyte_htable_higher_increase_shares, nt, type, time); 

  }

  if(type == LRV)
  {
    //inititialze the word sbox
    #if TRNG==1
      reset_systick();
      begin1 = SysTick->VAL; // Obtains the start time
    #endif // TRNG
    #if TRNG==0
        clock_gettime(CLOCK_REALTIME, &begin);
    #endif // TRNG
    init_table_sbox();
    gen_t_for_all_higher_increasing_shares(n,type,tim);
    #if TRNG==1
      end1 = SysTick->VAL; // Obtains the stop time
      time[0] = (double) (begin1-end1); // Calculates the time taken
      //time[0] =tim[0];
    #endif // TRNG
    #if TRNG==0
        clock_gettime(CLOCK_REALTIME, &end);
        sec = end.tv_sec - begin.tv_sec;
        nsec = end.tv_nsec - begin.tv_nsec;
        temp = sec + nsec*1e-9;

        time[0] = temp*UNIT/nt;
    #endif // TRNG
    
    run_aes_third(in, out, key, n, &subbyte_htable_higher_lrv, nt, type, time); 

  }
	
}

void run_aes_shares_third(byte *in, byte *out, byte *key, int n, int type, int nt, double time[11])
{
	
	unsigned int begin1, end1, begin2, end2;
	long sec, nsec;
	double temp = 0.0;
	double time_spent = 0.0;
  #if TRNG==0
    struct timespec begin, end;
	
	#endif
	
    		
        #if TRNG==1
            reset_systick();
            begin1 = SysTick->VAL; // Obtains the start time
            #endif // TRNG
        #if TRNG==0
        clock_gettime(CLOCK_REALTIME, &begin);
        #endif // TRNG
        gen_t_forall_third(n, type);
       
        #if TRNG==1
            end1 = SysTick->VAL; // Obtains the stop time
            time[0] = (double) (begin1-end1); // Calculates the time taken
            #endif // TRNG
         
        #if TRNG==0
          clock_gettime(CLOCK_REALTIME, &end);
          sec = end.tv_sec - begin.tv_sec;
          nsec = end.tv_nsec - begin.tv_nsec;
          temp = sec + nsec*1e-9;

          time[0] = temp*UNIT/nt;
        #endif // TRNG
        
		//printf("\n \n Online Phase\n\n\n");
        run_aes_third(in, out, key, n, &subbyte_htable_third, nt, type, time);	
		
	
 
	
	
}

/***************************AES_RP********************************************/
void mixcolumns_share1(byte stateshare[16][4],int n)
{
  byte ns[16];
  int i,j;
  for(i=0;i<n;i++)
  {
    for(j=0;j<4;j++)
    {
      ns[j*4]=multx(stateshare[j*4][i]) ^ multx(stateshare[j*4+1][i]) ^ stateshare[j*4+1][i] ^ stateshare[j*4+2][i] ^ stateshare[j*4+3][i];
      ns[j*4+1]=stateshare[j*4][i] ^ multx(stateshare[j*4+1][i]) ^ multx(stateshare[j*4+2][i]) ^ stateshare[j*4+2][i] ^ stateshare[j*4+3][i];
      ns[j*4+2]=stateshare[j*4][i] ^ stateshare[j*4+1][i] ^ multx(stateshare[j*4+2][i]) ^ multx(stateshare[j*4+3][i]) ^ stateshare[j*4+3][i];
      ns[j*4+3]=multx(stateshare[j*4][i]) ^ stateshare[j*4][i] ^ stateshare[j*4+1][i] ^ stateshare[j*4+2][i] ^ multx(stateshare[j*4+3][i]) ;
    }
    for(j=0;j<16;j++)
      stateshare[j][i]=ns[j];
  }
}
void shiftrows_share1(byte stateshare[16][4],int n)
{
  byte m;
  int i;
  for(i=0;i<n;i++)
  {
    m=stateshare[1][i];
    stateshare[1][i]=stateshare[5][i];
    stateshare[5][i]=stateshare[9][i];
    stateshare[9][i]=stateshare[13][i];
    stateshare[13][i]=m;

    m=stateshare[2][i];
    stateshare[2][i]=stateshare[10][i];
    stateshare[10][i]=m;
    m=stateshare[6][i];
    stateshare[6][i]=stateshare[14][i];
    stateshare[14][i]=m;

    m=stateshare[3][i];
    stateshare[3][i]=stateshare[15][i];
    stateshare[15][i]=stateshare[11][i];
    stateshare[11][i]=stateshare[7][i];
    stateshare[7][i]=m;
  }
}
void subbytestate_share_compress(byte *stateshare[16],int n,int l,byte *t1,byte *r,byte* y1,void (*subbyte_share_call_compress)(byte *,byte,byte,byte *,byte *,byte*))
{
  int i;
  for(i=0;i<16;i++)
    subbyte_share_call_compress(stateshare[i],l,n,t1,r,y1);
}
void addroundkey_share1(byte stateshare[16][4],int round,int n)
{
  int i,j;
  for(i=0;i<16;i++)
    for(j=0;j<n;j++)
      stateshare[i][j]^=wshare[16*round+i][j];
}


void subbytestate_share_RP(byte stateshare[16][4],int n,int j)//j indicates round number
{
		int i;
			for(i=0;i<16;i++)
			{
				subbyte_rp_share(stateshare[i],n);
			}
}

void aes_share_subkeys_RP(byte in[16],byte out[16],int n)
{
		int i;
		int round=0;

		byte stateshare[16][4];

		for(i=0;i<16;i++)
		{
			share_rnga(in[i],stateshare[i],n);
		}

		addroundkey_share1(stateshare,0,n);

		for(round=1;round<10;round++)
		{
    
				subbytestate_share_RP(stateshare,n,(round-1));
				shiftrows_share1(stateshare,n);
				mixcolumns_share1(stateshare,n);
				addroundkey_share1(stateshare,round,n);
		}

		subbytestate_share_RP(stateshare,n,(round-1));
		shiftrows_share1(stateshare,n);
		addroundkey_share1(stateshare,10,n);

		for(i=0;i<16;i++)
		{
			out[i]=decode(stateshare[i],n);
		}
}
void run_aes_share_RP(byte in[16],byte out[16],byte key[16],int n,int nt)
{
		int i;
		byte w[176];
    unsigned int begin1=0, end1=0;
	  double time_spent=0.0;
	  long sec,nsec;
    double temp=0.0;
   
   
      keyexpansion(key,w);

      for(i=0;i<176;i++)
      {
        share_rnga(w[i],wshare[i],n);
      }

      
      aes_share_subkeys_RP(in,out,n);
    
    
    
}


