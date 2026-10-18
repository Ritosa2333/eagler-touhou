/* Entry: 0x004c6b30; symbol: FUN_004c6b30; body bytes: 1584 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004c6b30(void)

{
  bool bVar1;
  int *piVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  DName *pDVar3;
  int iVar4;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined3 extraout_var_03;
  undefined1 local_7c [12];
  undefined1 local_70 [12];
  undefined1 local_64 [12];
  float *local_58;
  float *local_54;
  float *local_50;
  wchar_t *local_4c;
  int local_48;
  int local_44;
  int local_40;
  int local_3c;
  wchar_t *local_38;
  int local_34;
  int local_30;
  int local_2c;
  int local_28;
  int local_24;
  float local_20;
  float local_1c;
  undefined4 local_18;
  float local_14;
  float local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00488b00(DAT_005c0698,7);
  local_14 = *(float *)(local_24 + 0xe0);
  local_10 = *(float *)(local_24 + 0xe4);
  local_c = *(undefined4 *)(local_24 + 0xe8);
  FUN_00422e10(&local_20);
  FUN_004b8620(DAT_005c0698,0,1);
  FUN_00470ad0(DAT_005c0698,0xffffffff);
  FUN_00470ab0(DAT_005c0698,0xff000000);
  local_14 = local_14 + DAT_00571058;
  FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"Key Config");
  local_10 = local_10 + DAT_0056fd48;
  FUN_00470ad0(DAT_005c0698,0xff808080);
  FUN_00470ab0(DAT_005c0698,0xff000000);
  piVar2 = (int *)FUN_004c5e20(DAT_005b8898,*(int *)(local_24 + 0x10c));
  bVar1 = FUN_004c5e50(piVar2);
  if (CONCAT31(extraout_var,bVar1) == 0) {
    piVar2 = (int *)FUN_004c5e20(DAT_005b8898,*(int *)(local_24 + 0x10c));
    bVar1 = FUN_004c5eb0(piVar2);
    if (CONCAT31(extraout_var_00,bVar1) == 0) {
      local_30 = local_24 + 0xa0;
      FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"KeyBoard");
    }
    else {
      local_30 = local_24 + 0x80;
      FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"XInput");
    }
  }
  else {
    local_30 = local_24 + 0x60;
    FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"DirectInput");
  }
  FUN_00470ad0(DAT_005c0698,0xffffffff);
  local_14 = local_14 - DAT_00571054;
  local_10 = local_10 + DAT_0056fd48 + _DAT_00571050;
  FUN_004b8620(DAT_005c0698,1,1);
  pDVar3 = (DName *)FUN_004c5e20(DAT_005b8898,*(int *)(local_24 + 0x10c));
  iVar4 = DName::isEmpty(pDVar3);
  if (iVar4 == 0) {
    local_34 = 6;
  }
  else {
    local_34 = 9;
  }
  local_2c = local_34;
  for (local_28 = 0; local_28 < local_2c; local_28 = local_28 + 1) {
    local_20 = local_14;
    local_1c = local_10;
    local_18 = local_c;
    iVar4 = FUN_0040ff90(local_24 + 0xec);
    FUN_0046b590(&local_14,local_28,(void *)(local_24 + 0x14),0,iVar4);
    pDVar3 = (DName *)FUN_004c5e20(DAT_005b8898,*(int *)(local_24 + 0x10c));
    iVar4 = DName::isEmpty(pDVar3);
    if (iVar4 == 0) {
      local_38 = (wchar_t *)(&PTR_DAT_005afe10)[local_28];
    }
    else {
      local_38 = (wchar_t *)(&PTR_DAT_005afe28)[local_28];
    }
    FUN_0046c990(DAT_005c0698,&local_14,local_38);
    pDVar3 = (DName *)FUN_004c5e20(DAT_005b8898,*(int *)(local_24 + 0x10c));
    iVar4 = DName::isEmpty(pDVar3);
    if (iVar4 == 0) {
      local_3c = local_2c + -2;
    }
    else {
      local_3c = local_2c + -1;
    }
    if (local_28 < local_3c) {
      local_14 = local_14 + _DAT_0057105c;
      FUN_004b8620(DAT_005c0698,0,1);
      local_48 = (int)*(short *)(local_30 + local_28 * 2);
      pDVar3 = (DName *)FUN_004c5e20(DAT_005b8898,*(int *)(local_24 + 0x10c));
      iVar4 = DName::isEmpty(pDVar3);
      if (iVar4 == 0) {
        piVar2 = (int *)FUN_004c5e20(DAT_005b8898,*(int *)(local_24 + 0x10c));
        bVar1 = FUN_004c5e50(piVar2);
        if (CONCAT31(extraout_var_01,bVar1) == 0) {
          local_40 = 2;
        }
        else {
          local_40 = 1;
        }
        local_44 = local_40;
      }
      else {
        local_44 = 0;
      }
      local_4c = (wchar_t *)FUN_00421890(local_44,local_48);
      FUN_0046c990(DAT_005c0698,&local_14,local_4c);
      FUN_004b8620(DAT_005c0698,1,1);
    }
    local_14 = local_20;
    local_c = local_18;
    local_10 = local_1c + DAT_0056fd48;
  }
  FUN_00488b00(DAT_005c0698,0);
  FUN_00470ad0(DAT_005c0698,0xffffffff);
  FUN_004c6180(DAT_005c0698,3);
  FUN_00470ad0(DAT_005c0698,0xffc0c080);
  FUN_004c6160(DAT_005c0698,0xffffffff);
  FUN_00470ab0(DAT_005c0698,0xff000000);
  FUN_004b8620(DAT_005c0698,0,0);
  piVar2 = (int *)FUN_004c5e20(DAT_005b8898,*(int *)(local_24 + 0x10c));
  bVar1 = FUN_004c5e50(piVar2);
  if (CONCAT31(extraout_var_02,bVar1) == 0) {
    piVar2 = (int *)FUN_004c5e20(DAT_005b8898,*(int *)(local_24 + 0x10c));
    bVar1 = FUN_004c5eb0(piVar2);
    if (CONCAT31(extraout_var_03,bVar1) == 0) {
      local_54 = (float *)FUN_00422dd0(local_70,*(float *)(local_24 + 0xe0) + DAT_00571058,
                                       *(float *)(local_24 + 0xe4) + DAT_0056fe88,0);
      FUN_0040ddd0(&stack0xffffff78,&DAT_00571030);
      FUN_0046c210(DAT_005c0698,local_54);
      local_58 = (float *)FUN_00422dd0(local_7c,*(float *)(local_24 + 0xe0) + DAT_00571058,
                                       *(float *)(local_24 + 0xe4) + DAT_00571060,0);
      iVar4 = FUN_0040c300((undefined4 *)(local_24 + 0x14));
      FUN_0040ddd0(&stack0xffffff78,(&PTR_DAT_005afe64)[iVar4]);
      FUN_0046c210(DAT_005c0698,local_58);
      goto LAB_004c7109;
    }
  }
  local_50 = (float *)FUN_00422dd0(local_64,*(float *)(local_24 + 0xe0) + DAT_00571058,
                                   *(float *)(local_24 + 0xe4) + DAT_0056f10c,0);
  iVar4 = FUN_0040c300((undefined4 *)(local_24 + 0x14));
  FUN_0040ddd0(&stack0xffffff78,(&PTR_DAT_005afe4c)[iVar4]);
  FUN_0046c210(DAT_005c0698,local_50);
LAB_004c7109:
  FUN_00470ad0(DAT_005c0698,0xffffffff);
  FUN_004b8d90(DAT_005c0698,DAT_0056c8cc,DAT_0056c8cc);
  FUN_004b8620(DAT_005c0698,1,1);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

