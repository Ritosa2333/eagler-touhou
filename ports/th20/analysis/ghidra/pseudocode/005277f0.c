/* Entry: 0x005277f0; symbol: FUN_005277f0; body bytes: 1793 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_005277f0(int param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  int *piVar2;
  int iVar3;
  size_t sVar4;
  undefined3 extraout_var_00;
  float10 fVar5;
  float fVar6;
  int local_8c;
  int local_88;
  tm local_80;
  undefined4 local_5c;
  float local_58;
  tm local_50;
  undefined4 local_2c;
  float local_28;
  float local_20;
  float local_1c;
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (*(int *)(param_1 + 0x20) == 2) {
    FUN_00422dd0(&local_2c,DAT_0056ed10,DAT_0056fa30,0);
    FUN_004e6a20(DAT_005c0698,1);
    for (local_8c = 0; local_8c < 0x19; local_8c = local_8c + 1) {
      bVar1 = FUN_0046b1b0((void *)(param_1 + 0x24),local_8c);
      if (CONCAT31(extraout_var,bVar1) == 0) {
        FUN_00470ad0(DAT_005c0698,0xff808080);
      }
      else {
        FUN_00470ad0(DAT_005c0698,0xffffff00);
      }
      piVar2 = (int *)FUN_00414580((void *)(param_1 + 0x5740),local_8c);
      if (*piVar2 == 0) {
        FUN_0046c990(DAT_005c0698,&local_2c,
                     (wchar_t *)"No.%.2d -------- --/--/-- --:-- ------ ------- ------- --- ---%%");
      }
      else {
        piVar2 = (int *)FUN_00414580((void *)(param_1 + 0x5740),local_8c);
        iVar3 = FUN_00421810(*piVar2);
        local_50.tm_sec = 0;
        local_50.tm_min = 0;
        local_50.tm_hour = 0;
        local_50.tm_mday = 0;
        local_50.tm_mon = 0;
        local_50.tm_year = 0;
        local_50.tm_wday = 0;
        local_50.tm_yday = 0;
        local_50.tm_isdst = 0;
        FUN_004ac270(&local_50,(uint *)(iVar3 + 0x10));
        FUN_0046c990(DAT_005c0698,&local_2c,
                     (wchar_t *)"No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%");
      }
      local_28 = local_28 + _DAT_0056f2fc;
    }
    FUN_00470ad0(DAT_005c0698,0xffffffff);
    FUN_004e6a20(DAT_005c0698,0);
  }
  else if (*(int *)(param_1 + 0x20) == 3) {
    FUN_00422dd0(&local_5c,DAT_0056ed10,DAT_0056f108,0);
    bVar1 = FUN_00423560((void *)(param_1 + 0x154),10);
    if (bVar1) {
      fVar6 = (float)(*(int *)(param_1 + 0x5738) * 0xf + 0x50) - DAT_0056f108;
      fVar5 = FUN_00423bd0(param_1 + 0x154);
      local_58 = (fVar6 * (DAT_0056e734 - (float)fVar5)) / DAT_0056e734 + DAT_0056f108;
    }
    iVar3 = FUN_00421810(DAT_005c60fc);
    FUN_004ac270(&local_80,(uint *)(iVar3 + 0x10));
    FUN_0046c990(DAT_005c0698,&local_5c,
                 (wchar_t *)"No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%");
    bVar1 = FUN_004235c0((void *)(param_1 + 0x154),10);
    if (bVar1) {
      sVar4 = _strlen(PTR_s_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef_005aff7c);
      FUN_00422dd0(local_14,DAT_0056f090,DAT_0056f108,0);
      FUN_00470ad0(DAT_005c0698,0xffffffff);
      FUN_0046c990(DAT_005c0698,local_14,L"猥");
      local_14[0] = (float)(*(int *)(param_1 + 0x56e0) * 9) + local_14[0];
      if (*(int *)(param_1 + 0x56e0) == 8) {
        local_14[0] = local_14[0] - DAT_0056f7a4;
      }
      FUN_00470ad0(DAT_005c0698,0xffffff00);
      FUN_0046c990(DAT_005c0698,local_14,L"_");
      FUN_00470ad0(DAT_005c0698,0xffffffff);
      local_14[0] = DAT_0056fb7c;
      FUN_00422dd0(&local_20,DAT_00575674,DAT_0056fe88,0);
      for (local_88 = 0; local_88 < (int)sVar4; local_88 = local_88 + 1) {
        bVar1 = FUN_0046b1b0((void *)(param_1 + 0x56e8),local_88);
        if (CONCAT31(extraout_var_00,bVar1) == 0) {
          FUN_00470ad0(DAT_005c0698,0xff808080);
        }
        else {
          FUN_00470ad0(DAT_005c0698,0xffffff00);
        }
        if (local_88 < (int)(sVar4 - 3)) {
          FUN_0046c990(DAT_005c0698,&local_20,L"挥");
        }
        else {
          FUN_0046c990(DAT_005c0698,&local_20,L"挥");
        }
        if (local_88 % 0xd == 0xc) {
          local_20 = DAT_0056f10c - DAT_00572658;
          local_1c = local_1c + DAT_0056cd90;
        }
        else {
          local_20 = local_20 + _DAT_00572644;
        }
      }
      FUN_00470ad0(DAT_005c0698,0xffffffff);
    }
    FUN_00470ad0(DAT_005c0698,0xffffffff);
    FUN_004e6a20(DAT_005c0698,0);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

