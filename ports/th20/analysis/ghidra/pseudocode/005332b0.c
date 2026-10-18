/* Entry: 0x005332b0; symbol: FUN_005332b0; body bytes: 252 */

void __fastcall FUN_005332b0(void *param_1)

{
  bool bVar1;
  int iVar2;
  void *this;
  int iVar3;
  int local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00421680((int)param_1);
  iVar2 = FUN_00464080(0);
  iVar2 = FUN_00412730(iVar2);
  FUN_00534080(param_1,iVar2);
  iVar2 = FUN_00488830(DAT_005ba828);
  if (iVar2 != 0) {
    local_14[1] = 0;
    FUN_00461510(local_14 + 2);
    FUN_004615d0(local_14);
    while( true ) {
      bVar1 = FUN_00449460(local_14 + 2,local_14);
      if (!bVar1) break;
      iVar2 = FUN_0040c300(local_14 + 2);
      this = (void *)FUN_00464080(0);
      iVar3 = FUN_00421810(DAT_005c60fc);
      FUN_00534730(this,iVar2,*(undefined1 *)(iVar3 + 0xec + iVar2));
      FUN_00461860(local_14 + 2);
    }
  }
  FUN_00464230(0);
  FUN_005333b0();
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

