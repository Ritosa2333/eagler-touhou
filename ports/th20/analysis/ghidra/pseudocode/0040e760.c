/* Entry: 0x0040e760; symbol: FUN_0040e760; body bytes: 95 */

undefined1 __cdecl FUN_0040e760(undefined4 *param_1,undefined4 *param_2)

{
  bool bVar1;
  void *this;
  int iVar2;
  int iVar3;
  
  this = (void *)FUN_0040ff90((int)param_1);
  iVar2 = FUN_0040ff90((int)param_2);
  bVar1 = FUN_0040e710(this,iVar2);
  if (bVar1) {
    iVar2 = FUN_0040c300(param_1);
    iVar3 = FUN_0040c300(param_2);
    if (iVar2 == iVar3) {
      return 1;
    }
  }
  return 0;
}

