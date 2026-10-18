/* Entry: 0x004a5640; symbol: FUN_004a5640; body bytes: 1371 */

void __fastcall FUN_004a5640(void *param_1)

{
  bool bVar1;
  char cVar2;
  void *pvVar3;
  int iVar4;
  float *pfVar5;
  undefined4 *puVar6;
  uint uVar7;
  int *unaff_FS_OFFSET;
  float10 fVar8;
  undefined4 local_cc;
  int local_c8 [10];
  int *local_a0;
  int local_9c;
  void *local_98;
  void *local_94;
  undefined4 local_90;
  void *local_8c;
  void *local_88;
  float *local_84;
  void *local_80;
  void *local_7c;
  int local_78;
  void *local_74;
  int local_70;
  void *local_6c;
  float *local_68;
  void *local_64;
  void *local_60;
  undefined4 local_5c;
  undefined4 local_58;
  int local_54;
  float *local_50;
  float local_4c;
  int local_48;
  float local_44;
  int local_40;
  uint *local_3c;
  undefined4 *local_38;
  void *local_34;
  float local_30;
  int local_2c;
  uint local_28;
  char local_21;
  void *local_20;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056978d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_34 = (void *)((int)param_1 + 0x3cc);
  local_20 = param_1;
  FUN_0040c080(local_1c,8);
  FUN_00412280(local_34,local_1c);
  local_8 = 0;
  local_a0 = (int *)FUN_00412540();
  while (bVar1 = FUN_00411bb0(local_1c,local_a0), bVar1) {
    local_38 = (undefined4 *)FUN_0040c300(local_1c);
    pvVar3 = (void *)FUN_0040c300(local_38);
    FUN_004a5640(pvVar3);
    iVar4 = FUN_0040c300(local_38);
    local_3c = (uint *)(iVar4 + 0x354);
    *local_3c = *local_3c | 0x200;
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  FUN_00411ce0((int)local_20 + 0x3e4);
  if (-1 < *(int *)((int)local_20 + 0x2d8)) {
    fVar8 = FUN_004292e0((float *)((int)local_20 + 0x198));
    local_44 = (float)fVar8;
    local_48 = *(int *)((int)local_20 + 0x2d8);
    FUN_00426eb0(&DAT_005ba830,local_48,local_44);
  }
  pfVar5 = (float *)FUN_0047a2c0((int)local_20);
  fVar8 = FUN_004560d0((float *)((int)local_20 + 0xf4),pfVar5);
  local_4c = (float)fVar8;
  if (DAT_0056fbf8 * DAT_0056fbf8 <= local_4c) {
    pfVar5 = (float *)FUN_0047a2c0((int)local_20);
    fVar8 = (float10)FUN_0047a2e0((float *)((int)local_20 + 0xf4),pfVar5);
    local_30 = (float)fVar8;
  }
  else {
    local_30 = DAT_0056e0f8 / DAT_0056c8d0;
  }
  if (-1 < *(int *)((int)local_20 + 0x2dc)) {
    local_60 = (void *)FUN_0041cad0(*(int *)((int)local_20 + 0x424));
    local_58 = FUN_00499430(local_20,*(uint *)((int)local_20 + 0x2e0));
    local_50 = (float *)FUN_0047a2c0((int)local_20);
    local_54 = *(int *)((int)local_20 + 0x2dc);
    puVar6 = FUN_004790e0(&local_cc,(uint *)0x0,local_54,local_50,local_30,-1,(undefined4 *)0x0);
    local_5c = *puVar6;
    FUN_00497d20(local_60,local_5c);
  }
  local_64 = (void *)FUN_00412730(*(int *)((int)local_20 + 0x424));
  FUN_004ab820(local_64,local_30);
  local_6c = (void *)((int)local_20 + 0x230);
  uVar7 = FUN_0049c0c0((int)local_20);
  local_21 = (char)uVar7;
  local_68 = (float *)FUN_0047a2c0((int)local_20);
  FUN_0048bf10(local_6c,local_68,local_21);
  uVar7 = FUN_004ab300((int)local_20);
  if ((uVar7 & 0xff) != 0) {
    iVar4 = FUN_00460830(0);
    local_74 = (void *)FUN_004aaa60(iVar4);
    local_70 = *(int *)((int)local_20 + 0x2f4);
    FUN_004fb3a0(local_74,local_70);
    local_c8[0] = 100;
    local_c8[1] = 0x96;
    local_c8[2] = 200;
    local_c8[3] = 0xfa;
    local_c8[4] = 300;
    local_c8[5] = 400;
    local_c8[6] = 500;
    local_c8[7] = 600;
    local_c8[8] = 700;
    local_c8[9] = 800;
    iVar4 = FUN_00460830(0);
    iVar4 = FUN_004aaa60(iVar4);
    iVar4 = FUN_00438520(iVar4);
    if (iVar4 < 10) {
      iVar4 = FUN_00460830(0);
      iVar4 = FUN_004aaa60(iVar4);
      local_40 = FUN_00438520(iVar4);
    }
    else {
      local_40 = 9;
    }
    local_78 = local_c8[local_40];
    local_2c = (*(int *)((int)local_20 + 0x300) * local_78) / 100;
    local_7c = (void *)FUN_00464080(0);
    FUN_004a9fb0(local_7c,local_2c);
    iVar4 = FUN_00513dd0();
    cVar2 = FUN_00485a40(iVar4);
    if (cVar2 == '\0') {
      local_80 = (void *)FUN_00464080(0);
      FUN_00477f60(local_80,local_2c);
    }
    local_28 = FUN_00423ee0(0x5ba4a8);
    local_28 = local_28 % 5;
    local_88 = (void *)FUN_00464230(0);
    local_84 = (float *)FUN_0047a2c0((int)local_20);
    FUN_004a9f80(local_88,local_84,2000,local_28 + 9);
    iVar4 = FUN_00513dd0();
    cVar2 = FUN_00485a40(iVar4);
    if (cVar2 == '\0') {
      if (local_28 == 4) {
        local_28 = FUN_00423ee0(0x5ba4a8);
        local_28 = local_28 % 8;
        local_98 = (void *)FUN_00464080(0);
        FUN_004a9de0(local_98,local_28,local_2c);
      }
      else {
        local_94 = (void *)FUN_00464080(0);
        local_8c = (void *)FUN_00464080(0);
        local_90 = FUN_004641d0(local_8c,local_28);
        FUN_004a9de0(local_94,local_90,local_2c);
      }
    }
  }
  if (*(int *)((int)local_20 + 0x304) != -1) {
    FUN_004973c0((int)local_20);
    FUN_004972c0((int)local_20);
    FUN_005401d0(local_20,*(undefined4 *)((int)local_20 + 0x304));
    FUN_0053e2b0(local_20,0.0);
    *(undefined4 *)((int)local_20 + 0x304) = 0xffffffff;
    if ((*(uint *)((int)local_20 + 0x354) >> 0x1b & 1) != 0) goto LAB_004a5b82;
  }
  bVar1 = FUN_004145a0((int)local_20 + 0x3f8);
  if (bVar1) {
    local_9c = (int)local_20 + 0x3f8;
    FUN_004a3f20(local_9c);
  }
LAB_004a5b82:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

