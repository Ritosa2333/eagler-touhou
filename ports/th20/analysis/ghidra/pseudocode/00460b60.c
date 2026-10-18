/* Entry: 0x00460b60; symbol: FUN_00460b60; body bytes: 964 */

undefined4 __thiscall FUN_00460b60(void *this,undefined4 *param_1,undefined4 param_2)

{
  char cVar1;
  undefined4 *puVar2;
  int iVar3;
  char *pcVar4;
  int iVar5;
  float *pfVar6;
  void *this_00;
  void *this_01;
  void *this_02;
  float10 fVar7;
  float *pfVar8;
  float local_84 [2];
  void *local_7c;
  void *local_78;
  void *local_74;
  void *local_70;
  float *local_6c;
  void *local_68;
  float local_64;
  float local_60;
  float local_5c;
  float *local_58;
  float local_54;
  float local_50;
  void *local_4c;
  float local_48;
  float *local_44;
  float local_40;
  void *local_3c;
  void *local_38;
  void *local_34;
  void *local_30;
  void *local_2c;
  void *local_28;
  void *local_24;
  void *local_20;
  void *local_1c;
  int local_18;
  int local_14;
  int local_10;
  void *local_c;
  undefined1 local_5;
  
  *(undefined4 *)((int)this + 0x3e0) = param_2;
  local_1c = (void *)((int)this + 8);
  local_c = this;
  puVar2 = (undefined4 *)FUN_0045d670(local_1c,0);
  *puVar2 = 0;
  local_20 = (void *)((int)local_c + 8);
  iVar3 = FUN_0045d670(local_20,0);
  *(undefined4 *)(iVar3 + 4) = 0;
  *(undefined4 *)((int)local_c + 0x3cc) = param_1[7];
  *(undefined4 *)((int)local_c + 0x3e4) = *param_1;
  *(undefined4 *)((int)local_c + 1000) = param_1[1];
  *(undefined4 *)((int)local_c + 0x3ec) = param_1[2];
  *(undefined1 *)((int)local_c + 0x3f0) = *(undefined1 *)(param_1 + 10);
  *(undefined4 *)((int)local_c + 0x3c8) = param_1[6];
  FUN_00423520((void *)((int)local_c + 0x3d0),1);
  local_18 = 0;
  for (local_14 = 0; local_14 < 0x50; local_14 = local_14 + 1) {
    local_10 = 0x4f - local_14;
    local_24 = (void *)((int)local_c + 0x288);
    iVar3 = FUN_00414580(local_24,local_10);
    *(undefined1 *)(iVar3 + 2) = 0xff;
    local_28 = (void *)((int)local_c + 0x288);
    cVar1 = (char)local_14;
    pcVar4 = (char *)FUN_00414580(local_28,local_10);
    *pcVar4 = ('P' - cVar1) * '\x03';
    local_5 = FUN_00449d20(0x5ba4c4);
    local_2c = (void *)((int)local_c + 0x288);
    iVar3 = FUN_00414580(local_2c,local_10);
    *(undefined1 *)(iVar3 + 1) = local_5;
    local_30 = (void *)((int)local_c + 0x288);
    cVar1 = (char)local_14;
    iVar3 = FUN_00414580(local_30,local_10);
    *(char *)(iVar3 + 3) = ('P' - cVar1) * '\x03';
    if (local_10 < 0x29) {
      local_34 = (void *)((int)local_c + 0x288);
      iVar3 = local_18 << 5;
      iVar5 = FUN_00414580(local_34,local_10);
      *(char *)(iVar5 + 3) = -1 - (char)iVar3;
      local_18 = local_18 + 1;
    }
    if (local_10 < 8) {
      local_38 = (void *)((int)local_c + 0x288);
      iVar5 = local_14 << 5;
      iVar3 = FUN_00414580(local_38,local_10);
      *(char *)(iVar3 + 3) = (char)iVar5;
    }
    if (local_10 < 0x40) {
      fVar7 = FUN_00429830(0x5ba4c4);
      local_48 = (float)fVar7;
      local_50 = local_48 * DAT_0056c8cc + DAT_0056c8e0;
      local_54 = *(float *)((int)local_c + 0x3c8);
      local_4c = (void *)((int)local_c + 8);
      local_58 = (float *)FUN_0045d670(local_4c,local_10);
      FUN_00439330(this_02,local_58,local_54,local_50);
      fVar7 = FUN_004297d0(0x5ba4c4);
      local_5c = (float)fVar7;
      local_60 = *(float *)((int)local_c + 0x3cc) - local_5c * DAT_0056ef28;
      local_64 = *(float *)((int)local_c + 0x3c8);
      fVar7 = (float10)FUN_00438600(local_64,local_60);
      *(float *)((int)local_c + 0x3c8) = (float)fVar7;
    }
    else {
      local_40 = *(float *)((int)local_c + 0x3c8);
      local_3c = (void *)((int)local_c + 8);
      local_44 = (float *)FUN_0045d670(local_3c,local_10);
      FUN_00439330(this_01,local_44,local_40,DAT_0056d7bc);
      fVar7 = (float10)FUN_00438600(*(float *)((int)local_c + 0x3c8),
                                    *(float *)((int)local_c + 0x3cc));
      *(float *)((int)local_c + 0x3c8) = (float)fVar7;
    }
    if (local_10 < 0x4f) {
      local_68 = (void *)((int)local_c + 8);
      pfVar6 = (float *)FUN_0045d670(local_68,local_10 + 1);
      local_70 = (void *)((int)local_c + 8);
      pfVar8 = local_84;
      local_6c = pfVar6;
      this_00 = (void *)FUN_0045d670(local_70,local_10);
      FUN_00439960(this_00,pfVar8,pfVar6);
    }
  }
  local_74 = *(void **)((int)local_c + 4);
  FUN_0045dd60(local_74,param_1);
  local_78 = *(void **)((int)local_c + 4);
  FUN_00450350(local_78,5);
  local_7c = *(void **)((int)local_c + 4);
  FUN_00438c60(local_7c,0x50,0,0xff,0);
  return 0;
}

