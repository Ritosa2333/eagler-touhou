/* Entry: 0x0052b480; symbol: FUN_0052b480; body bytes: 1116 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_0052b480(int param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  int iVar2;
  undefined1 local_98 [12];
  undefined1 local_8c [12];
  undefined1 local_80 [12];
  undefined1 local_74 [12];
  undefined1 local_68 [12];
  float *local_5c;
  undefined *local_58;
  float *local_54;
  undefined *local_50;
  float *local_4c;
  undefined *local_48;
  float *local_44;
  float *local_40;
  undefined *local_3c;
  void *local_38;
  int local_34;
  uint local_30;
  uint local_2c;
  int local_28;
  uint local_24;
  float local_20;
  float local_1c;
  undefined4 local_18;
  float local_14;
  float local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_34 = *(int *)(param_1 + 0x20);
  local_28 = param_1;
  if (local_34 == 1) {
    FUN_00422dd0(&local_14,DAT_00575664,DAT_00570ad0,0);
    FUN_00422e10(&local_20);
    FUN_00488b00(DAT_005c0698,4);
    FUN_004e6a20(DAT_005c0698,1);
    local_20 = local_14;
    local_1c = local_10;
    local_18 = local_c;
    for (local_24 = 0; (int)local_24 < 0x29; local_24 = local_24 + 1) {
      local_38 = (void *)(local_28 + 0x24);
      bVar1 = FUN_0046b1b0(local_38,local_24);
      if (CONCAT31(extraout_var,bVar1) == 0) {
        FUN_00470ad0(DAT_005c0698,0xff707070);
      }
      else {
        FUN_00470ad0(DAT_005c0698,0xffffffff);
      }
      iVar2 = FUN_0052ca70(DAT_005c6108,local_24);
      if (iVar2 == 0) {
        FUN_0040ddd0(&stack0xffffff60,"--");
        FUN_0046ce90(DAT_005c0698,&local_14);
      }
      else {
        FUN_0046cd40(DAT_005c0698,&local_14,local_24 + 1,2,' ');
      }
      if ((int)local_24 % 10 == 9) {
        local_14 = local_20;
        local_10 = local_10 + _DAT_00575654;
      }
      else {
        local_14 = local_14 + _DAT_00575658;
      }
    }
    FUN_00488b00(DAT_005c0698,0);
    FUN_004e6a20(DAT_005c0698,0);
    iVar2 = FUN_0052ca70(DAT_005c6108,*(uint *)(local_28 + 0x5900));
    local_30 = (uint)(iVar2 != 0);
    local_2c = local_30;
    FUN_004c6160(DAT_005c0698,0xffffffff);
    if (local_2c == 0) {
      FUN_00470ad0(DAT_005c0698,0xff808080);
    }
    else {
      FUN_00470ad0(DAT_005c0698,0xffffffff);
    }
    FUN_00470ab0(DAT_005c0698,0xff000000);
    FUN_004c6180(DAT_005c0698,5);
    FUN_004b8620(DAT_005c0698,0,1);
    if (local_2c == 0) {
      local_44 = (float *)FUN_00422dd0(local_74,DAT_0056f10c,DAT_00575688,0);
      FUN_0040ddd0(&stack0xffffff60,&DAT_005755f0);
      FUN_0046c210(DAT_005c0698,local_44);
    }
    else {
      local_3c = FUN_0052f540(*(int *)(local_28 + 0x5900));
      local_40 = (float *)FUN_00422dd0(local_68,DAT_0056f10c,DAT_00575688,0);
      FUN_0040ddd0(&stack0xffffff5c,"%s");
      FUN_0046c210(DAT_005c0698,local_40);
    }
    local_48 = FUN_0052f4e0(*(int *)(local_28 + 0x5900),local_2c,0);
    local_4c = (float *)FUN_00422dd0(local_80,DAT_0056f10c,DAT_0057568c,0);
    FUN_0040ddd0(&stack0xffffff5c,"%s");
    FUN_0046c210(DAT_005c0698,local_4c);
    local_50 = FUN_0052f4e0(*(int *)(local_28 + 0x5900),local_2c,1);
    local_54 = (float *)FUN_00422dd0(local_8c,DAT_0056f10c,DAT_00572660,0);
    FUN_0040ddd0(&stack0xffffff5c,"%s");
    FUN_0046c210(DAT_005c0698,local_54);
    local_58 = FUN_0052f4e0(*(int *)(local_28 + 0x5900),local_2c,2);
    local_5c = (float *)FUN_00422dd0(local_98,DAT_0056f10c,DAT_00575694,0);
    FUN_0040ddd0(&stack0xffffff5c,"%s");
    FUN_0046c210(DAT_005c0698,local_5c);
    FUN_004e67e0(DAT_005c0698);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

