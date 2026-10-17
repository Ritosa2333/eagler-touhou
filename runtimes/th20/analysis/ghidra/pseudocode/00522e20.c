/* Entry: 0x00522e20; symbol: FUN_00522e20; body bytes: 1559 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00522e20(int param_1)

{
  bool bVar1;
  int iVar2;
  char *pcVar3;
  undefined3 extraout_var;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  undefined1 local_b0 [12];
  undefined1 local_a4 [12];
  void *local_98;
  int local_94;
  int local_90;
  int local_8c;
  uint local_88;
  float *local_84;
  void *local_80;
  int local_7c;
  float *local_78;
  int local_74;
  undefined4 local_70;
  undefined4 local_6c;
  int local_68;
  size_t local_64;
  int local_60;
  int local_5c;
  int local_58;
  int local_54;
  tm local_50;
  undefined4 local_2c;
  float local_28;
  float local_20;
  float local_1c;
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_74 = *(int *)(param_1 + 0x20);
  local_58 = param_1;
  if (local_74 == 2) {
    FUN_004c6160(DAT_005c0698,0xffffffff);
    FUN_00470ab0(DAT_005c0698,0xff000000);
    FUN_004c6180(DAT_005c0698,4);
    FUN_004b8620(DAT_005c0698,0,1);
    local_78 = (float *)FUN_00422dd0(local_a4,DAT_0056f10c,DAT_0056fa30,0);
    iVar2 = FUN_00464080(0);
    iVar2 = FUN_00412730(iVar2);
    FUN_0040ddd0(&stack0xffffff40,(&PTR_DAT_005aff88)[iVar2]);
    FUN_0046c210(DAT_005c0698,local_78);
    local_80 = (void *)FUN_0051b960();
    iVar2 = FUN_00464080(0);
    local_7c = FUN_0041cab0(iVar2);
    local_84 = (float *)FUN_00422dd0(local_b0,DAT_0056f10c,DAT_0056fe7c,0);
    pcVar3 = (char *)FUN_004dfad0(local_80,local_7c);
    FUN_0040ddd0(&stack0xffffff40,pcVar3);
    FUN_0046c210(DAT_005c0698,local_84);
    FUN_004e67e0(DAT_005c0698);
    FUN_00422dd0(&local_2c,DAT_0056fc04,DAT_0056fb80,0);
    local_8c = FUN_004640a0(0x5ba568);
    FUN_004e6a20(DAT_005c0698,1);
    for (local_60 = 0; local_60 < 10; local_60 = local_60 + 1) {
      if (*(int *)(local_58 + 0x56e4) == 0) {
        iVar2 = FUN_0040c300((undefined4 *)(local_58 + 0x24));
        if (iVar2 == local_60) {
          FUN_00470ad0(DAT_005c0698,0xffffffff);
        }
        else {
          FUN_00470ad0(DAT_005c0698,0xff808040);
        }
      }
      else {
        local_88 = local_60 * -0x10 + 0xff;
        FUN_00470ad0(DAT_005c0698,local_88 | 0xffffff00);
      }
      local_90 = FUN_004b8340(0x5ba568);
      local_94 = FUN_004b7f70(0x5ba568);
      iVar4 = local_60 * 0x28;
      iVar5 = local_8c * 400;
      iVar2 = FUN_004bd460(DAT_005c6108,local_94,local_90);
      local_68 = iVar2 + 0x18 + iVar5 + iVar4;
      local_54 = local_68;
      if (*(int *)(local_68 + 0x18) == 0 && *(int *)(local_68 + 0x1c) == 0) {
        FUN_0046c990(DAT_005c0698,&local_2c,
                     (wchar_t *)"%2d  %s  %9llu%d  ----/--/-- --:--  Stage -  ---%%");
      }
      else {
        FUN_004ac270(&local_50,(uint *)(local_68 + 0x18));
        FUN_0046c990(DAT_005c0698,&local_2c,
                     (wchar_t *)"%2d  %s  %9llu%d  %.4d/%.2d/%.2d %.2d:%.2d  %s  %2.1f%%");
      }
      local_28 = local_28 + _DAT_00572644;
    }
    if (*(int *)(local_58 + 0x56e4) == 0) {
      local_64 = _strlen(PTR_s_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef_005aff7c);
      uVar6 = 0;
      iVar2 = FUN_0040c300((undefined4 *)(local_58 + 0x24));
      FUN_00422dd0(local_14,DAT_00575660,(float)iVar2 * _DAT_00572644 + DAT_0056fb80,uVar6);
      FUN_00470ad0(DAT_005c0698,0xffffffff);
      FUN_0046c990(DAT_005c0698,local_14,L"猥");
      local_14[0] = (float)(*(int *)(local_58 + 0x56e0) * 9) + local_14[0];
      if (*(int *)(local_58 + 0x56e0) == 8) {
        local_14[0] = local_14[0] - DAT_0056f7a4;
      }
      FUN_00470ad0(DAT_005c0698,0xffffff00);
      FUN_0046c990(DAT_005c0698,local_14,L"_");
      FUN_00470ad0(DAT_005c0698,0xffffffff);
      local_14[0] = DAT_0056fb7c;
      FUN_00422dd0(&local_20,DAT_00575674,DAT_0056fe88,0);
      for (local_5c = 0; local_5c < (int)local_64; local_5c = local_5c + 1) {
        local_98 = (void *)(local_58 + 0x56e8);
        bVar1 = FUN_0046b1b0(local_98,local_5c);
        if (CONCAT31(extraout_var,bVar1) == 0) {
          FUN_00470ad0(DAT_005c0698,0xff808080);
        }
        else {
          FUN_00470ad0(DAT_005c0698,0xffffff00);
        }
        if (local_5c < (int)(local_64 - 3)) {
          FUN_0046c990(DAT_005c0698,&local_20,L"挥");
        }
        else {
          if (local_5c == local_64 - 3) {
            local_70 = 0x81;
          }
          else {
            if (local_5c == local_64 - 2) {
              local_6c = 0x7f;
            }
            else {
              local_6c = 0x80;
            }
            local_70 = local_6c;
          }
          FUN_0046c990(DAT_005c0698,&local_20,L"挥");
        }
        if (local_5c % 0xd == 0xc) {
          local_20 = DAT_0056f10c - DAT_00572658;
          local_1c = local_1c + DAT_0056cd90;
        }
        else {
          local_20 = local_20 + _DAT_00572644;
        }
      }
      FUN_00470ad0(DAT_005c0698,0xffffffff);
      FUN_004e6a20(DAT_005c0698,0);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

