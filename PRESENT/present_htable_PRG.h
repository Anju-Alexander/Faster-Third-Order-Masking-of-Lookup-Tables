#include "../Util/share.h"
#include "../Util/common.h"

void subbytestate_share_prg_present(byte stateshare[8][shares_N],int n,void (*subbyte_share_call)(byte *,int,int,int),int round, int type);


//*************************third order*********************
void gen_t_forall_present_third(int n, int type);
void subbyte_htable_present_third(byte *a,int n,int count,int type);

