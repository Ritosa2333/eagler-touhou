/* Entry: 0x00460240; symbol: FUN_00460240; body bytes: 810 */

undefined4 __fastcall FUN_00460240(int param_1)

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
  float local_7c [2];
  undefined4 *local_74;
  void *local_70;
  undefined4 *local_6c;
  void *local_68;
  float *local_64;
  void *local_60;
  float local_5c;
  float local_58;
  float local_54;
  void *local_50;
  float local_4c;
  float local_48;
  void *local_44;
  float local_40;
  float *local_3c;
  float local_38;
  void *local_34;
  uint local_30;
  void *local_2c;
  float local_28;
  float local_24;
  void *local_20;
  void *local_1c;
  char *local_18;
  int local_14;
  int local_10;
  int local_c;
  char local_5;
  
  local_1c = (void *)(param_1 + 0x3d0);
  local_c = param_1;
  bVar1 = FUN_00423560(local_1c,0x50);
  if (bVar1) {
    bVar1 = FUN_0045d030((int *)(local_c + 0x3d0));
    if (CONCAT31(extraout_var,bVar1) != 0) {
      local_10 = FUN_0040ff90(local_c + 0x3d0);
      if (0x31 < local_10) {
        for (local_14 = 0; local_14 < 0x50; local_14 = local_14 + 1) {
          local_20 = (void *)(local_c + 0x288);
          iVar2 = FUN_00414580(local_20,local_14);
          local_28 = (float)*(byte *)(iVar2 + 3);
          fVar5 = FUN_00429830(0x5ba4c4);
          local_24 = (float)fVar5;
          local_30 = (int)(local_28 / (local_24 * DAT_0056c8e0 + DAT_0056d7bc)) & 0xff;
          local_2c = (void *)(local_c + 0x288);
          iVar2 = FUN_00414580(local_2c,local_14);
          local_18 = (char *)(iVar2 + 3);
          local_5 = *local_18;
          *local_18 = local_5 - (char)local_30;
        }
      }
      if (local_10 < 0x10) {
        local_38 = *(float *)(local_c + 0x3c8);
        local_34 = (void *)(local_c + 8);
        local_3c = (float *)FUN_0045d670(local_34,local_10);
        FUN_00439330(this_00,local_3c,local_38,DAT_0056d7bc);
        fVar5 = (float10)FUN_00438600(*(float *)(local_c + 0x3c8),
                                      *(float *)(local_c + 0x3cc) -
                                      ((float)local_10 * *(float *)(local_c + 0x3cc)) / DAT_0056e04c
                                     );
        *(float *)(local_c + 0x3c8) = (float)fVar5;
      }
      else {
        fVar5 = FUN_00429830(0x5ba4c4);
        local_40 = (float)fVar5;
        local_48 = local_40 * DAT_0056c8cc + DAT_0056c8e0;
        local_4c = *(float *)(local_c + 0x3c8);
        local_44 = (void *)(local_c + 8);
        local_50 = (void *)FUN_0045d670(local_44,local_10);
        FUN_00439330(local_50,(float *)local_50,local_4c,local_48);
        fVar5 = FUN_004297d0(0x5ba4c4);
        local_54 = (float)fVar5;
        local_58 = *(float *)(local_c + 0x3cc) - local_54 * DAT_0056ef28;
        local_5c = *(float *)(local_c + 0x3c8);
        fVar5 = (float10)FUN_00438600(local_5c,local_58);
        *(float *)(local_c + 0x3c8) = (float)fVar5;
      }
      if (0 < local_10) {
        local_60 = (void *)(local_c + 8);
        pfVar3 = (float *)FUN_0045d670(local_60,local_10 + -1);
        local_68 = (void *)(local_c + 8);
        pfVar6 = local_7c;
        local_64 = pfVar3;
        this = (void *)FUN_0045d670(local_68,local_10);
        FUN_00439960(this,pfVar6,pfVar3);
      }
      iVar2 = FUN_00460830(0);
      if (iVar2 != 0) {
        local_70 = *(void **)(local_c + 4);
        iVar2 = FUN_00460830(0);
        local_6c = (undefined4 *)FUN_00460850(iVar2);
        FUN_0045dd60(local_70,local_6c);
      }
    }
    local_74 = (undefined4 *)(local_c + 0x3d0);
    FUN_00423540(local_74);
    uVar4 = 0;
  }
  else {
    uVar4 = 1;
  }
  return uVar4;
}

