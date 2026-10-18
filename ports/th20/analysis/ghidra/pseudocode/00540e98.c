/* Entry: 0x00540e98; symbol: FUN_00540e98; body bytes: 29 */

void FUN_00540e98(void)

{
  code *pcVar1;
  int local_10 [3];
  
  FUN_00540d5d(local_10);
  FUN_00544a7c(local_10,&DAT_005abff0);
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}

