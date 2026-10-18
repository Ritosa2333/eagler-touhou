/* Entry: 0x00410580; symbol: FUN_00410580; body bytes: 72 */

undefined1 __thiscall FUN_00410580(void *this,undefined4 *param_1,int param_2)

{
  bool bVar1;
  int iVar2;
  
  iVar2 = FUN_0040ff90((int)param_1);
  bVar1 = FUN_0040e710(this,iVar2);
  if ((bVar1) && (iVar2 = FUN_0040c300(param_1), iVar2 == param_2)) {
    return 1;
  }
  return 0;
}

