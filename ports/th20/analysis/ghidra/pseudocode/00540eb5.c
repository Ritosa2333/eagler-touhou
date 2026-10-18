/* Entry: 0x00540eb5; symbol: FUN_00540eb5; body bytes: 32 */

void FUN_00540eb5(int param_1)

{
  code *pcVar1;
  int local_10 [3];
  
  FUN_00540d8f(local_10,param_1);
  FUN_00544a7c(local_10,&DAT_005abf04);
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}

