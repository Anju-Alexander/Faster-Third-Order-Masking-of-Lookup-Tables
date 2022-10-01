#include <stdio.h>
#include<stdint.h>

#include "aes.h"
#include "aes_htable_prg.h"
#include "../Util/share.h"


#define TSIZE 256 //16 for PRESENT

#define K 256

typedef unsigned int word;

typedef uint32_t word;
int w1=4;
//common for every scheme
byte x_shares[160*(shares_N-1)];
byte T2[160*TSIZE];
//for the aes-basic scheme
byte y_shares[160*(shares_N - 2)];

byte Y3[40960]; // Y3 array, with 256*160=40960 size

//for higher order aes lrv
word T2_cw[10240][shares_N];
word sbox_Word[64];  //TSIZE/w=4

//for higher order aes
byte T2_c[40960][shares_N];

//*****************specific to AES third order PRG*********************//

void refresh_mask_sni(byte y_cw[4])
{
   byte t[4];
   gen_rand(t,4);
   y_cw[0]=(t[0]^y_cw[0])^t[1];
   y_cw[1]=(t[1]^y_cw[1])^t[2];
   y_cw[2]=(t[2]^y_cw[2])^t[3];
   y_cw[3]=(t[3]^y_cw[3])^t[0];
 
}

void refresh_mask_byte_LR(byte y_cw[4])
{
   byte r[3];
   gen_rand(r,3);
   y_cw[0]=y_cw[0]^r[0]^y_cw[1]^r[1]^y_cw[2]^r[2]^y_cw[3];
   y_cw[1]=r[0];
   y_cw[2]=r[1];
   y_cw[3]=r[2];

}

void fullrefresh_sni(byte a[shares_N])
{
  int i,j,n=shares_N;
  byte tmp[1];
  for(i=0;i<n;i++)
  {
  for(j=i+1;j<n;j++)
  {
    gen_rand(tmp,1); //rand();
    a[i]=a[i] ^ tmp[0];
    a[j]=a[j] ^ tmp[0];
  }
  }
}

void refresh_mask_byte(int ct,byte y_cw[4])
{ 
   byte y_t[4],r[ct];
   y_t[0]=y_cw[0];
   gen_rand(r,ct);
   for(int i=1;i<ct+1;i++)
   {
	   y_cw[i]=y_cw[i]^r[i-1];
	   y_cw[0]=y_cw[0]^r[i-1];
   }

}

//******************************************************************

void subbytestate_share_prg(byte stateshare[16][shares_N],int n,void (*subbyte_share_call)(byte *,int,int),int round)
{
  unsigned int t,ind;

  for(int i=0;i<16;i++)
  {
	ind=16*round+i;
	t=ind*(shares_N-1);
    byte temp=0;

    for(int j=0;j<n-1;j++)
       temp=temp ^ stateshare[i][j] ^ x_shares[t+j];

    stateshare[i][n-1]=stateshare[i][n-1] ^ temp;
    subbyte_share_call(stateshare[i],n,ind);

  }

}

void subbytestate_share_third(byte stateshare[16][shares_N], int n, void(*subbyte_share_call)(byte *, int, int, int), int round, int choice)
{
	unsigned int i, j;
	unsigned int t, ind;
  
	for (i = 0; i < 16; i++)
	{
		ind = 16*round + i;
		t = ind*(shares_N - 1);
		byte temp = 0;
		for (j = 0; j < n - 1; j++)
			temp = temp ^ stateshare[i][j] ^ x_shares[t + j];
    
	
		stateshare[i][n - 1] = stateshare[i][n - 1] ^ temp;
	
		subbyte_share_call(stateshare[i], n, ind, choice); 
	
	}

}

void subbyte_htable_third(byte y[shares_N], int n, int ind, int choice)
{

	byte x4;
	
	unsigned int t2 = ind*TSIZE;
	x4 = y[n - 1];
    
	y[0] = T2[t2 + x4]; //y4
	if(choice==BASIC)
	{
		unsigned int t1 = ind*(shares_N - 2);
		for (int i = 1; i < shares_N - 1; i++) //n=4, thus y0 and y1
		{
		y[i] = y_shares[t1 + i - 1]; //y1 y2
        }
	    y[n - 1] = Y3[t2 + x4];
	}
	
   #if SNI_RM==0
   refresh_mask_sni(y);
   #endif
   #if SNI_RM==1
   fullrefresh_sni(y);
   #endif
    
}

void htable_third(int n, int count, int choice)
{
	unsigned int j, i, t, t1, temp, temp1, k = 0;
	byte Tp[TSIZE], v[1], d, b;
    
	temp = count*TSIZE;
    
	t = count*(shares_N - 1);
	t1 = count*(shares_N - 2);
	temp1 = x_shares[t];
	for (j = 0; j < TSIZE; j++)
	{
		Tp[j] = sbox[j^temp1] ^ y_shares[t1]; //S[x+a]+y1
	}
	gen_rand(v, 1);
    
	d = (x_shares[t + 1] ^ v[0]) ^ x_shares[t + 2];
    
	for (j = 0; j < TSIZE; j++)
	{
		b = j ^ d;
		T2[temp + b] = (Tp[v[0] ^ j] ^ Y3[temp + b]) ^ y_shares[t1 + 1];  
	}
    
}

void gen_t_forall_third(int n, int choice)
{
	unsigned int i, j, temp1, temp2, temp3;
	byte a[shares_N - 1],  c[TSIZE], common;

	
	for (i = 0; i < 160; i++)
	{
		gen_rand(a, n - 1);
		temp1 = i*(shares_N - 1);

		for (j = 0; j < (n - 1); j++)
		{
			x_shares[temp1 + j] = a[j]; //need for both the choices
     
		}
        if (choice == BASIC)
		{
			byte b[shares_N - 2];
			
			gen_rand(b, n - 2);
		    temp2 = i*(shares_N - 2);
		    for (j = 0; j < (n - 1); j++)
		    {
				y_shares[temp2 + j] = b[j]; 
            
	     	}
		    gen_rand(c, TSIZE);
			temp3 = i*TSIZE;
			for (j = 0; j < TSIZE; j++)
			{
				Y3[temp3 + j] = c[j]; 
            
			}
			htable_third(n, i, choice);
		}
	
		
        
	}

}

//**********************************specific to AES LRV**********************************
void init_table_sbox()
{
  for(int k=0;k<TSIZE/w1; k++)
  {
    word r=0;
    for(int j=w1-1;j>=0;j--)
    {
      r=r << 8;
      r^=sbox[k*w1+j];
    }
    sbox_Word[k]=r;
    
  }
}


/****************specific to higher order increasing shares******************/
void refresh_mask(int ct,word y_cw[4])
{
   word r;
   word r1[ct];
   //gen_rand32_arr(r1,ct);
   for(int i=1;i<ct+1;i++)
   {
	   r=gen_rand32();
	   y_cw[i]=y_cw[i]^r;
	   y_cw[0]=y_cw[0]^r;
   }
 
}




void subbyte_htable_higher_lrv(byte y[shares_N], int n, int ind, int choice)
{
	int i,j;
	byte x4=y[n-1];
	unsigned int t=ind*(TSIZE/w1),tmp=ind*(shares_N-1);
	byte val=0;
	int k1=6,k2=2;
	n=shares_N;
	byte xl,x4_m=x4>>k2,x4_l=x4&3;
	byte T2_small[w1][n];
	byte T1_s[4][4];

	refresh_mask(n-1,T2_cw[t+(x4_m)]);

	
	for(int i=0;i<w1;i++)
	{
		for(int j=0;j<n;j++)
		{
		T1_s[i][j]=(T2_cw[t+(x4_m)][j]>>(i*8))&0xff;
		}
	}


	for(j=0;j<n-1;j++)
	{
		xl=(x_shares[tmp+j])&3;
		for(i=0;i<w1;i++)
		{
			for(int l=0;l<n;l++)
			T2_small[i][l]=T1_s[i^xl][l];
			
		}
		for(i=0;i<w1;i++)
		{
			for(int l=0;l<n;l++)
			T1_s[i][l]=T2_small[i][l];
			refresh_mask_byte(j+1,T1_s[i]);
		}
		
	}
    refresh_mask_byte(n-1,T1_s[i]);
	//refresh_mask_sni(T1_s[i]);
	for(int l=0;l<n;l++)
	y[l]=T1_s[x4_l][l];
	
}

void htable_gen_higher_lrv(int ind)
{
	
	int size=TSIZE/w1;
	int k1=6,k2=2,n=shares_N,i,j,l;
	unsigned int t=ind*(size);
	unsigned int temp=ind*(shares_N-1),temp2=ind*size,temp3=ind*TSIZE;
	word T1_small[64][shares_N],temp1; //
    byte xm;
	
	for(i=0;i<size;i++)
	{
		T2_cw[t+i][0]=sbox_Word[i];
		T1_small[i][0]=0;
	    for(j=1;j<n;j++)
		{
			T2_cw[t+i][j]=0;
			T1_small[i][j]=0;
		}
	}

	for(j=0;j<n-1;j++)
	{
		xm=(x_shares[temp+j])>>k2;
		for(i=0;i<size;i++)
		{
			for(l=0;l<=j;l++)
			T1_small[i][l]=T2_cw[t+(i^xm)][l];

		}
		for(i=0;i<size;i++)
		{
			for(l=0;l<n;l++)
			T2_cw[t+i][l]=T1_small[i][l];
            refresh_mask(j+1,T2_cw[t+i]);
		}

	}
	
}
void subbyte_htable_higher_increase_shares(byte y[shares_N], int n, int ind, int choice)
{
	byte x4=y[n-1];
	unsigned int temp=ind*(shares_N-1),tmp=ind*TSIZE;
	refresh_mask_sni(T2_c[tmp+x4]);
	for(int i=0;i<n;i++)
	{
		y[i]=T2_c[tmp+x4][i];
	}
	
}

void htable_gen_higher_increase_shares(int ind)
{
	int n=shares_N, tmp=ind*TSIZE,temp1=ind*(shares_N-1);
	int i,j,l;
	byte T_aux[TSIZE][n],x;


	for(j=0;j<TSIZE;j++)
	{
		T2_c[tmp+j][0]=sbox[j];
		T_aux[j][0]=0;
		for(l=1;l<n;l++)
		{
			T2_c[tmp+j][l]=0;
			T_aux[j][l]=0; //T_aux needs to be initialized to 0 as otherwise junk values might be present
		}
	}
	for(i=0;i<n-1;i++)
	{
	    x=x_shares[temp1+i];
	   for(j=0;j<TSIZE;j++)
	   {
		   for(l=0;l<=i;l++)
		   T_aux[j][l]=T2_c[tmp+j^x][l];

	    }
	    for(j=0;j<TSIZE;j++)
	    {
			for(l=0;l<n;l++)
            T2_c[tmp+j][l]=T_aux[j][l];
			refresh_mask_byte(i+1,T2_c[tmp+j]);
	    }
	}

	
}

void gen_t_for_all_higher_increasing_shares(int n, int choice, double tim[1])
{
	n=shares_N;
	unsigned int temp1;
	byte a[n-1];
	unsigned int begin1, end1, begin2, end2;
	tim[0]=0.0;
	for(int i=0;i<160;i++)
	{
		temp1=i*(shares_N-1);
	    gen_rand(a,n-1);
		for(int j=0;j<n-1;j++)
		x_shares[temp1+j]=a[j];	
		
		if(choice==BASIC)
		{
			htable_gen_higher_increase_shares(i);
		}
        if(choice==LRV)
		{
			htable_gen_higher_lrv(i);
		}
	

	}   

}

