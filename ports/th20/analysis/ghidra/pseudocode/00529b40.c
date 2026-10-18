/* Entry: 0x00529b40; symbol: FUN_00529b40; body bytes: 825 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00529b40(int param_1)

{
  bool bVar1;
  char cVar2;
  undefined3 extraout_var;
  int iVar3;
  int iVar4;
  int local_18;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if ((*(int *)(param_1 + 0x20) == 2) || (*(int *)(param_1 + 0x20) == 3)) {
    FUN_00422dd0(&local_14,DAT_0056fa30,DAT_0056fa30,0);
    FUN_00488b00(DAT_005c0698,7);
    bVar1 = FUN_004235c0((void *)(param_1 + 0x154),10);
    if ((bVar1) || (*(int *)(param_1 + 0x20) == 3)) {
      local_14 = DAT_00575684;
      local_10 = DAT_00575668;
      for (local_18 = 1; local_18 < 7; local_18 = local_18 + 1) {
        bVar1 = FUN_0046b1b0((void *)(param_1 + 0x24),local_18 + -1);
        if (CONCAT31(extraout_var,bVar1) == 0) {
          FUN_00470ad0(DAT_005c0698,0xff808080);
        }
        else {
          iVar3 = FUN_0050fc50(DAT_005c6108);
          iVar4 = FUN_004640a0(0x5ba568);
          cVar2 = FUN_0052ca30(iVar3 + 0x76f8 + iVar4 * 0x90 + (local_18 + -1) * 0x10);
          if (cVar2 == '\0') {
            FUN_00470ad0(DAT_005c0698,0xffdfdfdf);
          }
          else if ((*(int *)(param_1 + 0x20) == 3) &&
                  (iVar3 = FUN_00472040((void *)(param_1 + 0x154),4), 1 < iVar3)) {
            FUN_00470ad0(DAT_005c0698,0xff000000);
          }
          else {
            FUN_00470ad0(DAT_005c0698,0xffffff00);
          }
        }
        iVar3 = FUN_0050fc50(DAT_005c6108);
        iVar4 = FUN_004640a0(0x5ba568);
        cVar2 = FUN_0052ca30(iVar3 + 0x76f8 + iVar4 * 0x90 + (local_18 + -1) * 0x10);
        if (cVar2 == '\0') {
          FUN_0046c990(DAT_005c0698,&local_14,L"猥");
          local_14 = local_14 + DAT_00570ad4;
          FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"---------");
          local_14 = local_14 - DAT_00570ad4;
        }
        else {
          FUN_0046c990(DAT_005c0698,&local_14,L"猥");
          local_14 = local_14 + DAT_00570ad4;
          FUN_0050fc50(DAT_005c6108);
          FUN_004640a0(0x5ba568);
          FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"%.7d0");
          local_14 = local_14 - DAT_00570ad4;
        }
        local_10 = local_10 + _DAT_00575654;
      }
    }
    FUN_00470ad0(DAT_005c0698,0xffffffff);
    FUN_004e6a20(DAT_005c0698,0);
    FUN_00488b00(DAT_005c0698,0);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

