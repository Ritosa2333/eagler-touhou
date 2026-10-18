/* Entry: 0x00508480; symbol: FUN_00508480; body bytes: 1661 */

/* WARNING: Type propagation algorithm not settling */

void __thiscall FUN_00508480(void *this,undefined4 param_1,undefined1 (*param_2) [16])

{
  bool bVar1;
  short sVar2;
  int iVar3;
  undefined4 *puVar4;
  int *piVar5;
  undefined4 uVar6;
  undefined4 *puVar7;
  int *piVar8;
  undefined4 local_2c8 [139];
  void *local_9c;
  int local_98;
  void *local_94;
  undefined4 local_90;
  undefined4 local_8c;
  undefined4 *local_88;
  void *local_84;
  void *local_80;
  undefined4 local_7c;
  uint local_78;
  undefined4 local_74;
  undefined4 local_70;
  void *local_6c;
  undefined4 local_68;
  void *local_64;
  undefined4 local_60;
  void *local_5c;
  undefined4 *local_58;
  undefined4 *local_54;
  uint local_50;
  int *local_4c;
  int local_48;
  int local_44;
  int local_40;
  int local_3c;
  undefined1 local_36;
  undefined1 local_35;
  int *local_34;
  void *local_30;
  undefined4 local_2c;
  int local_28;
  int local_24 [3];
  int local_18;
  uint local_14;
  
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  *(undefined4 *)((int)this + 0x10) = param_1;
  local_30 = this;
  if (*(int *)((int)this + 0x10) == 0) {
    iVar3 = FUN_00474d60(0x5ba568);
    Concurrency::details::ContextBase::ClearCancel
              ((ContextBase *)((int)local_30 + iVar3 * 0x14 + 0x40));
    puVar4 = FUN_00507000();
    *(undefined4 **)((int)local_30 + 0x18) = puVar4;
    *(undefined4 *)(*(int *)((int)local_30 + 0x18) + 0x1c) = 0x30;
    *(undefined4 *)(*(int *)((int)local_30 + 0x18) + 0x20) = 0x100;
    *(undefined4 *)(*(int *)((int)local_30 + 0x18) + 0x24) = 0x2a0;
    puVar4 = FUN_00506f80();
    *(undefined4 **)((int)local_30 + 0x1c) = puVar4;
    local_88 = FUN_00507080();
    local_5c = (void *)((int)local_30 + 0x20);
    iVar3 = FUN_00474d60(0x5ba568);
    puVar4 = (undefined4 *)FUN_00414580(local_5c,iVar3);
    *puVar4 = local_88;
    local_84 = (void *)((int)local_30 + 0x20);
    iVar3 = FUN_00474d60(0x5ba568);
    puVar4 = (undefined4 *)FUN_00414580(local_84,iVar3);
    local_34 = (int *)*puVar4;
    local_60 = FUN_004b7f70(0x5ba568);
    *(undefined4 *)(*(int *)((int)local_30 + 0x1c) + 0xd8) = local_60;
    local_24[1] = 0;
    local_4c = local_24 + 1;
    FUN_00461510(&local_18);
    FUN_004615d0(local_24);
    while (bVar1 = FUN_00449460(&local_18,local_24), bVar1) {
      local_40 = FUN_0040c300(&local_18);
      local_64 = (void *)FUN_00464080(0);
      local_68 = FUN_0050a280(local_64,local_40);
      *(undefined4 *)(*(int *)((int)local_30 + 0x1c) + 0xdc + local_40 * 4) = local_68;
      local_6c = (void *)FUN_00464080(0);
      local_36 = FUN_00464420(local_6c,local_40);
      *(undefined1 *)(*(int *)((int)local_30 + 0x1c) + 0xec + local_40) = local_36;
      FUN_00461860(&local_18);
    }
    local_70 = FUN_004640a0(0x5ba568);
    *(undefined4 *)(*(int *)((int)local_30 + 0x1c) + 0xf0) = local_70;
    local_35 = FUN_004bd820(0x5ba568);
    *(byte *)(*(int *)((int)local_30 + 0x1c) + 10) =
         *(byte *)(*(int *)((int)local_30 + 0x1c) + 10) & 0xfe | local_35;
    bVar1 = FUN_00488850(0x5ba568);
    local_50 = (uint)bVar1;
    *(byte *)(*(int *)((int)local_30 + 0x1c) + 10) =
         *(byte *)(*(int *)((int)local_30 + 0x1c) + 10) & 0xfd | bVar1 << 1;
    local_74 = FUN_00499460(0x5ba568);
    *(undefined4 *)(*(int *)((int)local_30 + 0x1c) + 0xfc) = local_74;
    if (DAT_005ba828 != 0) {
      puVar4 = (undefined4 *)(DAT_005ba828 + 0x38);
      puVar7 = (undefined4 *)(*(int *)((int)local_30 + 0x1c) + 0x20);
      for (iVar3 = 0x2c; iVar3 != 0; iVar3 = iVar3 + -1) {
        *puVar7 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar7 = puVar7 + 1;
      }
    }
    sVar2 = FUN_00474d60(0x5ba568);
    *local_34 = (int)sVar2;
    iVar3 = FUN_0041ca90(0x5ba4a8);
    local_34[1] = iVar3;
    std::_Adl_verify_range<char*,char_const*>((char **)"RandomSeed %d\n",(char **)local_34[1]);
    FUN_0049da50((undefined4 *)&DAT_005ba4a8);
    local_78 = FUN_0049c000();
    local_34[0xa6] = local_34[0xa6] & 0xfffffffeU | local_78 & 1;
    iVar3 = FUN_0049c000();
    if (iVar3 != 0) {
      local_34[4] = 0;
      local_34[5] = 0;
    }
    piVar5 = (int *)FUN_00498f40();
    piVar8 = local_34 + 0x1c;
    for (iVar3 = 0x8a; iVar3 != 0; iVar3 = iVar3 + -1) {
      *piVar8 = *piVar5;
      piVar5 = piVar5 + 1;
      piVar8 = piVar8 + 1;
    }
    for (local_3c = 0; local_3c < 0x14; local_3c = local_3c + 1) {
      local_34[local_3c + 7] = local_3c * -0x21522153;
    }
    local_7c = FUN_004b7fa0(0x5ba568);
    *(undefined4 *)(*(int *)((int)local_30 + 0x1c) + 0xf8) = local_7c;
    piVar5 = FUN_00412310(0x15,FUN_00509f60,local_30);
    *(int **)((int)local_30 + 8) = piVar5;
    piVar5 = FUN_00412310(0x2e,FUN_00509f70,local_30);
    *(int **)((int)local_30 + 0x254) = piVar5;
    piVar5 = FUN_004123b0(0x5b,FUN_0050a000,local_30);
    *(int **)((int)local_30 + 0xc) = piVar5;
    uVar6 = FUN_00474d60(0x5ba568);
    *(undefined4 *)((int)local_30 + 600) = uVar6;
    *(undefined4 *)((int)local_30 + 0x250) = 0xffffffff;
  }
  else if (*(int *)((int)this + 0x10) == 1) {
    iVar3 = FUN_00508b90(this,param_2);
    if (iVar3 == 0) {
      puVar4 = (undefined4 *)(*(int *)((int)local_30 + 0x1c) + 0x20);
      puVar7 = (undefined4 *)(DAT_005ba828 + 0x38);
      for (iVar3 = 0x2c; iVar3 != 0; iVar3 = iVar3 + -1) {
        *puVar7 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar7 = puVar7 + 1;
      }
      local_80 = (void *)((int)local_30 + 0xe8);
      iVar3 = FUN_00474d60(0x5ba568);
      local_54 = (undefined4 *)FUN_00486e30(local_80,iVar3);
      local_48 = local_54[4];
      FUN_00509e10(local_54);
      FUN_0050a3e0(&DAT_005ba568,*(undefined4 *)(*(int *)((int)local_30 + 0x1c) + 0xd8));
      local_44 = 0;
      local_2c = 0;
      local_58 = &local_2c;
      FUN_00461510(local_24 + 2);
      FUN_004615d0(&local_28);
      while (bVar1 = FUN_00449460(local_24 + 2,&local_28), bVar1) {
        local_90 = FUN_0040c300(local_24 + 2);
        local_94 = (void *)FUN_00464080(0);
        local_8c = *(undefined4 *)(*(int *)((int)local_30 + 0x1c) + 0xdc + local_44 * 4);
        local_44 = local_44 + 1;
        FUN_0050a870(local_94,local_90,local_8c);
        FUN_00461860(local_24 + 2);
      }
      local_9c = (void *)FUN_00464230(0);
      iVar3 = FUN_00464080(0);
      local_98 = FUN_00412730(iVar3);
      FUN_00534080(local_9c,local_98);
      FUN_004be1b0(&DAT_005ba568,*(undefined4 *)(*(int *)((int)local_30 + 0x1c) + 0xf0));
      FUN_004d9940(&DAT_005ba4a8,*(undefined4 *)(local_48 + 4));
      std::_Adl_verify_range<char*,char_const*>
                ((char **)"RandomSeed %d\n",*(char ***)(local_48 + 4));
      FUN_0049da50((undefined4 *)&DAT_005ba4a8);
      puVar4 = (undefined4 *)(local_48 + 0x70);
      puVar7 = local_2c8;
      for (iVar3 = 0x8a; iVar3 != 0; iVar3 = iVar3 + -1) {
        *puVar7 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar7 = puVar7 + 1;
      }
      puVar7 = (undefined4 *)FUN_00498f40();
      puVar4 = local_2c8;
      for (iVar3 = 0x8a; iVar3 != 0; iVar3 = iVar3 + -1) {
        *puVar7 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar7 = puVar7 + 1;
      }
      iVar3 = FUN_00499460(0x5ba568);
      if (iVar3 < 0) {
        FUN_0050a4a0(&DAT_005ba568,0);
      }
      else {
        FUN_0050a4a0(&DAT_005ba568,2);
      }
      piVar5 = FUN_00412310(0x15,FUN_00509f50,local_30);
      *(int **)((int)local_30 + 8) = piVar5;
      piVar5 = FUN_00412310(0x2e,FUN_00509f70,local_30);
      *(int **)((int)local_30 + 0x254) = piVar5;
      piVar5 = FUN_004123b0(0x5b,FUN_0050a000,local_30);
      *(int **)((int)local_30 + 0xc) = piVar5;
      *(undefined4 *)((int)local_30 + 600) = 0xffffffff;
    }
  }
  else if (*(int *)((int)this + 0x10) == 2) {
    FUN_00508b90(this,param_2);
  }
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffff0);
  return;
}

