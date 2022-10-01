#include "../Util/common.h"
#include "../Util/share.h"



void subbytestate_share_prg(byte stateshare[16][shares_N],int n,void (*subbyte_share_call)(byte *,int,int),int round);
/***********specific to third order**************/
void subbyte_htable_third(byte y[shares_N], int n, int ind, int choice);
void gen_t_forall_third(int n, int choice);
void subbytestate_share_third(byte stateshare[16][shares_N], int n, void(*subbyte_share_call)(byte *, int, int, int), int round, int choice);

/*************specific to LRV*******************/
void init_table_sbox();
/*****************Corons scheme***********/

void gen_t_for_all_higher_increasing_shares(int n, int choice, double tim[1]);
void subbyte_htable_higher_lrv(byte y[shares_N], int n, int ind, int choice);
void subbyte_htable_higher_increase_shares(byte y[shares_N], int n, int ind, int choice);


