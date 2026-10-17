/* Entry: 0x00466480; symbol: FUN_00466480; body bytes: 520 */

undefined4 __fastcall FUN_00466480(int param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  int iVar2;
  float *pfVar3;
  void *this;
  undefined4 uVar4;
  void *this_00;
  float10 fVar5;
  float *pfVar6;
  float local_5c [2];
  undefined4 *local_54;
  float local_50;
  float local_4c;
  float local_48;
  void *local_44;
  float *local_40;
  void *local_3c;
  float *local_38;
  float local_34;
  float local_30;
  void *local_2c;
  float local_28;
  void *local_24;
  void *local_20;
  void *local_1c;
  void *local_18;
  char *local_14;
  int local_10;
  int local_c;
  int local_8;
  
  local_18 = (void *)(param_1 + 0x174);
  local_8 = param_1;
  bVar1 = FUN_00423560(local_18,0x1e);
  if (bVar1) {
    bVar1 = FUN_0045d030((int *)(local_8 + 0x174));
    if (CONCAT31(extraout_var,bVar1) != 0) {
      local_10 = FUN_0040ff90(local_8 + 0x174);
      for (local_c = 0; local_c < local_10; local_c = local_c + 1) {
        local_1c = (void *)(local_8 + 0xf8);
        iVar2 = FUN_00414580(local_1c,local_c);
        if (*(byte *)(iVar2 + 3) < 0x10) {
          local_24 = (void *)(local_8 + 0xf8);
          iVar2 = FUN_00414580(local_24,local_c);
          *(undefined1 *)(iVar2 + 3) = 0;
        }
        else {
          local_20 = (void *)(local_8 + 0xf8);
          iVar2 = FUN_00414580(local_20,local_c);
          local_14 = (char *)(iVar2 + 3);
          *local_14 = *local_14 + -0x10;
        }
      }
      fVar5 = FUN_00429830(0x5ba4c4);
      local_28 = (float)fVar5;
      local_30 = local_28 * DAT_0056d7bc + DAT_0056c8cc;
      local_34 = *(float *)(local_8 + 0x170);
      local_2c = (void *)(local_8 + 8);
      local_38 = (float *)FUN_0045d670(local_2c,local_10);
      FUN_00439330(this_00,local_38,local_34,local_30);
      if (0 < local_10) {
        local_3c = (void *)(local_8 + 8);
        pfVar3 = (float *)FUN_0045d670(local_3c,local_10 + -1);
        local_44 = (void *)(local_8 + 8);
        pfVar6 = local_5c;
        local_40 = pfVar3;
        this = (void *)FUN_0045d670(local_44,local_10);
        FUN_00439960(this,pfVar6,pfVar3);
      }
      fVar5 = FUN_004297d0(0x5ba4c4);
      local_48 = (float)fVar5;
      local_4c = local_48 / DAT_0056d7bc;
      local_50 = *(float *)(local_8 + 0x170);
      fVar5 = (float10)FUN_00438600(local_50,local_4c);
      *(float *)(local_8 + 0x170) = (float)fVar5;
    }
    local_54 = (undefined4 *)(local_8 + 0x174);
    FUN_00423540(local_54);
    uVar4 = 0;
  }
  else {
    uVar4 = 1;
  }
  return uVar4;
}

