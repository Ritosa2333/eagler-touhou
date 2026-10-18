/* Entry: 0x004f6c40; symbol: FUN_004f6c40; body bytes: 800 */

/* WARNING: Removing unreachable block (ram,0x004f6c9c) */

undefined4 * __thiscall FUN_004f6c40(void *this,undefined4 *param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int *unaff_FS_OFFSET;
  undefined1 local_cc [8];
  locale local_c4 [8];
  undefined4 local_bc;
  undefined4 local_b8;
  undefined4 local_b4;
  undefined4 local_b0;
  undefined4 local_ac;
  undefined4 local_a8;
  undefined4 local_a4;
  undefined4 local_a0;
  undefined4 local_9c;
  undefined4 local_98;
  undefined4 local_94;
  undefined4 local_90;
  undefined4 local_8c;
  undefined1 *local_88;
  undefined1 *local_84;
  undefined4 local_80;
  undefined4 local_7c;
  undefined1 *local_78;
  int local_74;
  undefined4 local_70;
  undefined4 local_6c;
  void *local_68;
  locale *local_64;
  locale *local_60;
  void *local_5c;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  int local_44;
  undefined1 *local_40;
  undefined1 *local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined1 local_28;
  undefined1 local_27;
  char local_26;
  char local_25;
  int *local_24;
  undefined1 *puStack_20;
  int local_1c;
  undefined1 *puStack_18;
  undefined4 local_14;
  
  puStack_20 = &stack0xfffffffc;
  local_14 = 0xffffffff;
  puStack_18 = &LAB_0056a4e0;
  local_1c = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_1c;
  local_2c = *(undefined4 *)((int)this + 8);
  local_25 = '\0';
  local_26 = *(char *)(*(int *)((int)this + 4) + 10);
  local_24 = (int *)this;
  puVar1 = (undefined4 *)move<>(&param_2);
  local_30 = *puVar1;
  puVar1 = FUN_004f1b60(&local_b8,local_30,local_26,local_25);
  param_2 = *puVar1;
  puVar1 = (undefined4 *)move<>(&param_2);
  local_38 = *puVar1;
  local_34 = local_38;
  iVar2 = FUN_0040c300((undefined4 *)local_24[3]);
  iVar3 = FUN_0040ff90(local_24[3]);
  local_3c = (undefined1 *)(iVar2 + iVar3);
  local_40 = (undefined1 *)FUN_0040c300((undefined4 *)local_24[3]);
  puVar1 = FUN_004ef4d0(&local_bc,local_40,local_3c,local_38);
  param_2 = *puVar1;
  if ((*(char *)local_24[4] != '\0') && (*(int *)local_24[5] < *(int *)local_24[1])) {
    local_27 = 0x30;
    local_44 = *(int *)local_24[1] - *(int *)local_24[5];
    puVar1 = (undefined4 *)move<>(&param_2);
    local_48 = *puVar1;
    puVar1 = (undefined4 *)FUN_004e6f50(&local_b4,local_48,local_44,&local_27);
    param_2 = *puVar1;
  }
  if (*(int *)local_24[6] < 1) {
    puVar1 = (undefined4 *)move<>(&param_2);
    local_80 = *puVar1;
    local_84 = (undefined1 *)*local_24;
    local_88 = *(undefined1 **)local_24[7];
    local_7c = local_80;
    FUN_004ef4d0(param_1,local_88,local_84,local_80);
  }
  else {
    puVar1 = (undefined4 *)move<>(&param_2);
    local_6c = *puVar1;
    local_70 = *(undefined4 *)local_24[6];
    local_5c = (void *)local_24[9];
    local_58 = local_6c;
    local_54 = local_6c;
    local_50 = local_6c;
    local_4c = local_6c;
    local_64 = FUN_004fbf20(local_5c,local_c4);
    local_14 = 0;
    local_60 = local_64;
    piVar4 = (int *)FUN_004f3910(local_64);
    local_28 = FUN_00500060(piVar4);
    local_68 = (void *)local_24[8];
    puVar1 = (undefined4 *)FUN_004494f0(local_68,local_cc);
    local_b0 = *puVar1;
    local_ac = puVar1[1];
    local_74 = *local_24;
    local_78 = *(undefined1 **)local_24[7];
    local_a8 = local_b0;
    local_a4 = local_ac;
    local_a0 = local_b0;
    local_9c = local_ac;
    local_98 = local_b0;
    local_94 = local_ac;
    local_90 = local_b0;
    local_8c = local_ac;
    FUN_004f19c0(param_1,local_78,local_74);
    local_14 = 0xffffffff;
    FUN_004f5600((int)local_c4);
  }
  *unaff_FS_OFFSET = local_1c;
  return param_1;
}

