/* Entry: 0x004c7770; symbol: FUN_004c7770; body bytes: 1335 */

void FUN_004c7770(void)

{
  int iVar1;
  undefined1 local_7c [12];
  undefined1 local_70 [12];
  undefined1 local_64 [12];
  float *local_58;
  float *local_54;
  float *local_50;
  uint local_4c;
  int local_48;
  uint local_44;
  int local_40;
  undefined *local_3c;
  undefined4 local_38;
  undefined *local_34;
  uint local_30;
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
  FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"Controller Select");
  local_1c = local_10 + DAT_0056fc04;
  local_20 = local_14;
  local_18 = local_c;
  local_10 = local_1c;
  local_2c = FUN_0040ff90(local_24 + 0xec);
  local_30 = FUN_0040ff90(local_24 + 0xd0);
  FUN_0046b590(&local_14,0,(void *)(local_24 + 0x14),local_30,local_2c);
  iVar1 = FUN_00421990(DAT_005b8898,0);
  if (iVar1 < 1) {
    iVar1 = FUN_00421990(DAT_005b8898,0);
    local_34 = (&PTR_s_Keyboard_005afe00)[iVar1];
    FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"<%s> Setting");
  }
  else {
    local_38 = FUN_00421990(DAT_005b8898,0);
    local_3c = PTR_s_GamePad_005afe04;
    FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"<%s%d> Setting");
  }
  local_1c = local_1c + DAT_0056fd48;
  local_14 = local_20;
  local_c = local_18;
  local_10 = local_1c;
  FUN_00470ad0(DAT_005c0698,0xffffffff);
  local_1c = local_1c + DAT_0056fd48;
  local_14 = local_20;
  local_c = local_18;
  local_10 = local_1c;
  FUN_00470ad0(DAT_005c0698,0xffffffff);
  local_40 = FUN_0040ff90(local_24 + 0xec);
  local_44 = FUN_0040ff90(local_24 + 0xd0);
  FUN_0046b590(&local_14,1,(void *)(local_24 + 0x14),local_44,local_40);
  FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"KeyConfig ALL Reset");
  local_1c = local_1c + DAT_0056fd48;
  local_14 = local_20;
  local_c = local_18;
  local_10 = local_1c;
  FUN_00470ad0(DAT_005c0698,0xffffffff);
  local_48 = FUN_0040ff90(local_24 + 0xec);
  local_4c = FUN_0040ff90(local_24 + 0xd0);
  FUN_0046b590(&local_14,2,(void *)(local_24 + 0x14),local_4c,local_48);
  FUN_0046c990(DAT_005c0698,&local_14,L"畑瑩");
  local_1c = local_1c + DAT_0056fa28;
  local_14 = local_20;
  local_c = local_18;
  local_10 = local_1c;
  FUN_00470ad0(DAT_005c0698,0xffa0a0b0);
  FUN_00470ab0(DAT_005c0698,0xff000000);
  FUN_00488b00(DAT_005c0698,0);
  FUN_00470ad0(DAT_005c0698,0xffffffff);
  FUN_004b8620(DAT_005c0698,1,1);
  FUN_004c6180(DAT_005c0698,3);
  FUN_00470ad0(DAT_005c0698,0xffc0c080);
  FUN_004c6160(DAT_005c0698,0xffffffff);
  FUN_00470ab0(DAT_005c0698,0xff000000);
  FUN_004b8620(DAT_005c0698,0,0);
  local_28 = FUN_0040c300((undefined4 *)(local_24 + 0x14));
  if (local_28 == 0) {
    local_50 = (float *)FUN_00422dd0(local_64,*(float *)(local_24 + 0xe0) + DAT_00571058,
                                     *(float *)(local_24 + 0xe4) + DAT_0056f10c,0);
    FUN_0040ddd0(&stack0xffffff7c,&DAT_00570ebc);
    FUN_0046c210(DAT_005c0698,local_50);
  }
  else if (local_28 == 1) {
    local_54 = (float *)FUN_00422dd0(local_70,*(float *)(local_24 + 0xe0) + DAT_00571058,
                                     *(float *)(local_24 + 0xe4) + DAT_0056ec9c,0);
    FUN_0040ddd0(&stack0xffffff7c,&DAT_00570edc);
    FUN_0046c210(DAT_005c0698,local_54);
  }
  else if (local_28 == 2) {
    local_58 = (float *)FUN_00422dd0(local_7c,*(float *)(local_24 + 0xe0) + DAT_00571058,
                                     *(float *)(local_24 + 0xe4) + DAT_0056ec9c,0);
    FUN_0040ddd0(&stack0xffffff7c,&DAT_00570efc);
    FUN_0046c210(DAT_005c0698,local_58);
  }
  FUN_00470ad0(DAT_005c0698,0xffffffff);
  FUN_004b8d90(DAT_005c0698,DAT_0056c8cc,DAT_0056c8cc);
  FUN_004b8620(DAT_005c0698,1,1);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

