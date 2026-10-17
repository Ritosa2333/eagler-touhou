/* Entry: 0x0051a0b0; symbol: FUN_0051a0b0; body bytes: 1490 */

/* WARNING: Type propagation algorithm not settling */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_0051a0b0(void *this,int param_1)

{
  char cVar1;
  bool bVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined1 local_100 [12];
  undefined1 local_f4 [12];
  undefined4 local_e8;
  undefined4 local_e4;
  undefined4 local_e0;
  uint local_dc;
  undefined4 local_d8;
  float *local_d4;
  undefined4 local_d0;
  void *local_cc;
  undefined4 local_c8;
  uint local_c4;
  void *local_c0;
  void *local_bc;
  undefined4 local_b8;
  void *local_b4;
  void *local_b0;
  undefined4 local_ac;
  uint local_a8;
  undefined4 local_a4;
  uint local_a0;
  undefined4 local_9c;
  uint local_98;
  undefined4 local_94;
  uint local_90;
  void *local_8c;
  undefined4 local_88;
  undefined4 local_84;
  int local_80;
  uint local_7c;
  undefined4 local_78;
  int local_74;
  int local_70;
  int *local_6c;
  undefined4 local_68;
  int *local_64;
  int local_60;
  void *local_5c;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_40;
  undefined4 local_3c;
  undefined1 local_38 [4];
  undefined4 local_34;
  int local_20 [4];
  int local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_60 = param_1;
  local_5c = this;
  if (param_1 == 0) {
    *(undefined4 *)((int)this + 0x14) = 1;
    iVar3 = FUN_004640a0(0x5ba568);
    if (iVar3 == 4) {
      while( true ) {
        iVar3 = FUN_00464080(0);
        local_c8 = FUN_00412730(iVar3);
        local_70 = FUN_00464100((int)DAT_005c6108);
        iVar3 = FUN_00464080(0);
        local_74 = FUN_00412730(iVar3);
        cVar1 = FUN_0051bc10(DAT_005c6108,local_74,local_70);
        if (cVar1 != '\0') break;
        iVar3 = FUN_00464080(0);
        local_78 = FUN_00412730(iVar3);
        iVar3 = FUN_0051ba40((int)DAT_005c6108);
        local_7c = iVar3 + 1U & 0x80000007;
        if ((int)local_7c < 0) {
          local_7c = (local_7c - 1 | 0xfffffff8) + 1;
        }
        iVar3 = FUN_00464080(0);
        local_80 = FUN_00412730(iVar3);
        FUN_0051c920(DAT_005c6108,0,local_80,local_7c);
      }
    }
    local_20[1] = 0;
    local_64 = local_20 + 1;
    FUN_00461510(&local_c);
    FUN_004615d0(local_20);
    while (bVar2 = FUN_00449460(&local_c,local_20), bVar2) {
      local_68 = FUN_0040c300(&local_c);
      local_8c = (void *)FUN_00464080(0);
      iVar3 = FUN_00464080(0);
      local_84 = FUN_00412730(iVar3);
      local_88 = FUN_00464100((int)DAT_005c6108);
      FUN_0051c8b0(local_8c,local_68,local_88);
      FUN_00461860(&local_c);
    }
    local_20[2] = 0;
    local_6c = local_20 + 2;
    FUN_00461510(&local_10);
    FUN_0050ae80(local_20 + 3);
    while (bVar2 = FUN_00449460(&local_10,local_20 + 3), bVar2) {
      local_90 = FUN_0040c300(&local_10);
      FUN_0051c7e0(DAT_005c6108,local_90,0);
      FUN_00461860(&local_10);
    }
    iVar3 = FUN_00464080(0);
    local_94 = FUN_00412730(iVar3);
    local_98 = FUN_00464100((int)DAT_005c6108);
    FUN_0051b0c0(DAT_005c6108,local_98);
    iVar3 = FUN_00464080(0);
    local_9c = FUN_00412730(iVar3);
    local_a0 = FUN_00464100((int)DAT_005c6108);
    FUN_0051b0c0(DAT_005c6108,local_a0);
    iVar3 = FUN_00464080(0);
    local_a4 = FUN_00412730(iVar3);
    local_a8 = FUN_00464100((int)DAT_005c6108);
    FUN_0051b0c0(DAT_005c6108,local_a8);
    FUN_0047ba30(&local_58);
    puVar4 = FUN_00422dd0(local_f4,_DAT_0057370c,DAT_0056f108,0);
    local_58 = *puVar4;
    local_54 = puVar4[1];
    local_50 = puVar4[2];
    local_40 = 0;
    local_3c = 0;
    FUN_0045d650(local_38,0xffff0000);
    local_34 = DAT_0056e050;
    local_ac = FUN_00437520(0);
    puVar4 = FUN_0049db70(&local_e8,0xc,&local_58,0);
    *(undefined4 *)((int)local_5c + 0xc4) = *puVar4;
    FUN_00423520((void *)((int)local_5c + 0x18),0);
    local_b0 = (void *)((int)local_5c + 0x28);
    FUN_0049c4e0(local_b0,5);
    local_b4 = (void *)((int)local_5c + 0xc4);
    FUN_0044ef90(local_b4,7);
    FUN_004bed50((void *)((int)local_5c + 0x28),0);
    local_bc = (void *)((int)local_5c + 0xc4);
    iVar3 = FUN_00464080(0);
    local_b8 = FUN_00412730(iVar3);
    iVar3 = FUN_0051ba40((int)DAT_005c6108);
    OnStatusTextChange(local_bc,*(undefined4 *)(&DAT_005745a0 + iVar3 * 4));
    local_cc = (void *)FUN_0044ced0((undefined4 *)((int)local_5c + 0xc4));
    local_c0 = (void *)FUN_00464080(0);
    iVar3 = FUN_004641d0(local_c0,0);
    local_c4 = *(uint *)(&DAT_005745a0 + iVar3 * 4);
    iVar3 = FUN_0044ced0((undefined4 *)((int)local_5c + 0xc4));
    local_dc = FUN_004456e0(iVar3);
    FUN_0051c6f0(local_cc,0x14,0,local_dc,local_c4);
    local_d0 = *(undefined4 *)((int)local_5c + 0x10);
    puVar4 = FUN_00450c70(&local_e0,(uint *)"stone",0x1b,-1,(undefined4 *)0x0);
    *(undefined4 *)((int)local_5c + 0xdc) = *puVar4;
    *(undefined4 *)((int)local_5c + 0x210f4) = 1;
  }
  else if (param_1 == 1) {
    FUN_00423520((void *)((int)this + 0x18),0);
    *(undefined4 *)((int)local_5c + 0x210f4) = 6;
  }
  else if (param_1 == 2) {
    FUN_00423520((void *)((int)this + 0x18),0);
    *(undefined4 *)((int)local_5c + 0x210f4) = 8;
    local_d8 = FUN_00411700((int)local_5c);
    local_d4 = (float *)FUN_00422dd0(local_100,DAT_0056cdb0,DAT_0056fa3c,0);
    puVar4 = FUN_00464600(&local_e4,(uint *)"stone",1,local_d4,(undefined4 *)0x0);
    *(undefined4 *)((int)local_5c + 0xe4) = *puVar4;
    FUN_00488920((void *)((int)local_5c + 0xe4));
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

