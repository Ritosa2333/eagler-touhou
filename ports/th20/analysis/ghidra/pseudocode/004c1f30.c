/* Entry: 0x004c1f30; symbol: FUN_004c1f30; body bytes: 138 */

void __fastcall FUN_004c1f30(char **param_1)

{
  int iVar1;
  uint uVar2;
  void *this;
  
  iVar1 = FUN_0040c300(param_1 + 0x23);
  if (iVar1 != 0) {
    uVar2 = FUN_0040c300(param_1 + 0x23);
    this = (void *)FUN_00412710((int)param_1[0x30]);
    FUN_004c1eb0(this,param_1);
    param_1[5] = (char *)((uint)param_1[5] & 0xfffffffe);
    FUN_004117a0(param_1 + 0x23,0);
    if ((uVar2 & 0x1000000) != 0) {
      FUN_00413b70(DAT_005b8894,param_1);
    }
  }
  return;
}

