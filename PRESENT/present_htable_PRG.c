#include <stdio.h>

//#include "../Util/prg3.h"
#include "../Util/share.h"

#include "present.h"
#include "present_htable_PRG.h"

#define P_TSIZE 16 //16 for PRESENT


byte p_x_shares[496*(shares_N-1)]; //Shares of x used as part of pre-processing.  496 *(N2-1) fpr PRESENT
byte p_y_shares[496*(shares_N-2)];
byte T2_p[496*P_TSIZE];
byte Y3_p[496*P_TSIZE];
//**************online functions**********

void present_refreshMask(byte a[4],int n)
{
    byte t=a[0];
	byte b[n-1];

    gen_rand(b,n);
    a[0]=b[0]^a[0]^b[1];
    a[1]=b[1]^a[1]^b[2];
    a[2]=b[2]^a[2]^b[3];
    a[3]=b[3]^a[3]^b[0];
}



void subbytestate_share_prg_present(byte stateshare[8][shares_N],int n,void (*subbyte_share_call)(byte *,int,int,int),int round, int type)
{
  unsigned int i,j;
  unsigned int t,ind[2];
  byte a[2][shares_N];

  /*for(j=0;j<2;j++)
        a[j]=(byte*) malloc(n*sizeof(byte));*/

  for(i=0;i<8;i++)
  {

	ind[0]=16*round+i;
	ind[1]=16*round+i+1;
	t=ind[0]*(n-1);
    byte temp[2]={0,0};

    for(j=0;j<n;j++)
    {
        a[0][j]=stateshare[i][j]>>4;
        a[1][j]=stateshare[i][j] & 0xF;
    }


    for(j=0;j<n-1;j++)
    {
        temp[0]=temp[0]^a[0][j]^p_x_shares[t+j];
        temp[1]=temp[1]^a[1][j]^p_x_shares[t+(n-1)+j];
    }

    a[0][n-1]=a[0][n-1]^temp[0];
    a[1][n-1]=a[1][n-1]^temp[1];


    subbyte_share_call(a[0],n,ind[0],type);
    subbyte_share_call(a[1],n,ind[1],type);

    for(j=0;j<n;j++)
    {
        stateshare[i][j]=a[0][j]<<4 | a[1][j];
    }

    //locality_refresh(stateshare[i],n);
   
    present_refreshMask(stateshare[i],n);
    
  }

}





//************End of functions for Increasing shares multi PRG*******************

//*********************third order*******************//

//**************online functions**********


void read_htable_present_third(byte a,byte *b,int n,int count,int type)
{
    unsigned int t=count*P_TSIZE;
    unsigned int t1=count*(n-1);
     
   byte x1= p_x_shares[t1], x2=p_x_shares[t1+1], x3=p_x_shares[t1+2];
   b[0]=T2_p[t+(a)];
   
   
    unsigned int t2=count*(n-2);
    for(int i=1;i<n-1;i++)
    {
        b[i]=p_y_shares[t2+i-1];
    }
    b[n-1]=Y3_p[t+a];
    

}


void subbyte_htable_present_third(byte *a,int n,int count,int type)
{
    read_htable_present_third(a[n-1],a,n,count, type);
}

///****************offline functions************//



void htable_m_present_third(int n,int count)
{
   unsigned int i, d, v, b;
   byte a[1];
   byte T1_p[P_TSIZE];
  
   unsigned int t=count*(n-1), t1=count*(n-2);
   byte x1=p_x_shares[t];
   byte x2=p_x_shares[t+1];
   byte x3=p_x_shares[t+2];
   gen_rand(a,1);
   v=a[0]%16;
   d= (x2 ^ v ) ^ x3;
   for(i=0;i<P_TSIZE;i++)
   {
       T1_p[i]=sbox_p[i ^ x1] ^ p_y_shares[t1];
   }
   unsigned temp=count*P_TSIZE;
   for(i=0;i<P_TSIZE;i++)
   {
       b=d^i;
       T2_p[temp+b]=(T1_p[v ^ i] ^ Y3_p[temp+b]) ^ p_y_shares[t1+1];
   }
}


void gen_t_forall_present_third(int n, int type)
{
    unsigned int i,j,temp;
    byte a[n-1], c[P_TSIZE];

   	for(i=0;i<496;i++)
    {
        gen_rand(a,n-1);
		temp=i*(n-1);

		for(j=0;j<n-1;j++)
        {
            p_x_shares[temp+j]=(a[j]%16);
   
        }
        
        byte b[n-2];
        gen_rand(b,n-2);
        temp=i*(n-2);

        for(j=0;j<n-2;j++)
        {
            p_y_shares[temp+j]=(b[j]%16);

        }
        gen_rand(c,P_TSIZE);
        temp=i*P_TSIZE;

        for(j=0;j<P_TSIZE;j++)
        {
        Y3_p[temp+j]=(c[j]%16);
        
        }

        htable_m_present_third(n,i);

    }

}

