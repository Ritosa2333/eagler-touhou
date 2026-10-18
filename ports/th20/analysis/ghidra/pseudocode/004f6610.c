/* Entry: 0x004f6610; symbol: FUN_004f6610; body bytes: 783 */

/* WARNING: Removing unreachable block (ram,0x004f665b) */

undefined4 * __thiscall FUN_004f6610(void *this,undefined4 *param_1,undefined4 param_2)

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
  undefined1 *local_84;
  undefined1 *local_80;
  undefined4 local_7c;
  undefined4 local_78;
  undefined1 *local_74;
  int local_70;
  undefined4 local_6c;
  undefined4 local_68;
  void *local_64;
  locale *local_60;
  locale *local_5c;
  void *local_58;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  int local_40;
  undefined1 *local_3c;
  undefined1 *local_38;
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
  local_25 = '\0';
  local_26 = *(char *)(*(int *)((int)this + 4) + 10);
  local_24 = (int *)this;
  puVar1 = (undefined4 *)move<>(&param_2);
  local_2c = *puVar1;
  puVar1 = FUN_004f1b60(&local_b8,local_2c,local_26,local_25);
  param_2 = *puVar1;
  puVar1 = (undefined4 *)move<>(&param_2);
  local_34 = *puVar1;
  local_30 = local_34;
  iVar2 = FUN_0040c300((undefined4 *)local_24[3]);
  iVar3 = FUN_0040ff90(local_24[3]);
  local_38 = (undefined1 *)(iVar2 + iVar3);
  local_3c = (undefined1 *)FUN_0040c300((undefined4 *)local_24[3]);
  puVar1 = FUN_004ef4d0(&local_bc,local_3c,local_38,local_34);
  param_2 = *puVar1;
  if ((*(char *)local_24[4] != '\0') && (*(int *)local_24[5] < *(int *)local_24[1])) {
    local_27 = 0x30;
    local_40 = *(int *)local_24[1] - *(int *)local_24[5];
    puVar1 = (undefined4 *)move<>(&param_2);
    local_44 = *puVar1;
    puVar1 = (undefined4 *)FUN_004e6f50(&local_b4,local_44,local_40,&local_27);
    param_2 = *puVar1;
  }
  if (*(int *)local_24[6] < 1) {
    puVar1 = (undefined4 *)move<>(&param_2);
    local_7c = *puVar1;
    local_80 = (undefined1 *)*local_24;
    local_84 = *(undefined1 **)local_24[7];
    local_78 = local_7c;
    FUN_004ef4d0(param_1,local_84,local_80,local_7c);
  }
  else {
    puVar1 = (undefined4 *)move<>(&param_2);
    local_68 = *puVar1;
    local_6c = *(undefined4 *)local_24[6];
    local_58 = (void *)local_24[9];
    local_54 = local_68;
    local_50 = local_68;
    local_4c = local_68;
    local_48 = local_68;
    local_60 = FUN_004fbf20(local_58,local_c4);
    local_14 = 0;
    local_5c = local_60;
    piVar4 = (int *)FUN_004f3910(local_60);
    local_28 = FUN_00500060(piVar4);
    local_64 = (void *)local_24[8];
    puVar1 = (undefined4 *)FUN_004494f0(local_64,local_cc);
    local_b0 = *puVar1;
    local_ac = puVar1[1];
    local_70 = *local_24;
    local_74 = *(undefined1 **)local_24[7];
    local_a8 = local_b0;
    local_a4 = local_ac;
    local_a0 = local_b0;
    local_9c = local_ac;
    local_98 = local_b0;
    local_94 = local_ac;
    local_90 = local_b0;
    local_8c = local_ac;
    FUN_004f19c0(param_1,local_74,local_70);
    local_14 = 0xffffffff;
    FUN_004f5600((int)local_c4);
  }
  *unaff_FS_OFFSET = local_1c;
  return param_1;
}

