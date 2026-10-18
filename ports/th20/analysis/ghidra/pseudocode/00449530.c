/* Entry: 0x00449530; symbol: FUN_00449530; body bytes: 52 */

int __fastcall FUN_00449530(undefined4 param_1)

{
  int iVar1;
  int *piVar2;
  
  piVar2 = (int *)move<>(param_1);
  LOCK();
  iVar1 = *piVar2;
  *piVar2 = *piVar2 + 1;
  UNLOCK();
  return iVar1;
}

