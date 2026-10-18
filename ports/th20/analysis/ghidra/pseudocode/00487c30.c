/* Entry: 0x00487c30; symbol: FUN_00487c30; body bytes: 1896 */

void __thiscall FUN_00487c30(void *this,int param_1,uint *param_2,undefined4 param_3,int param_4)

{
  bool bVar1;
  undefined3 extraout_var;
  int iVar2;
  char *pcVar3;
  undefined4 *puVar4;
  int iVar5;
  int *unaff_FS_OFFSET;
  undefined1 auStack_154 [16];
  undefined4 uStack_144;
  uint *puVar6;
  undefined1 local_124 [36];
  undefined4 local_100;
  undefined4 local_fc;
  undefined4 local_f8;
  undefined4 local_f4;
  undefined4 local_f0;
  undefined1 *local_ec;
  undefined4 local_e8;
  undefined4 local_e4;
  int local_e0 [6];
  undefined4 local_c8;
  undefined4 local_c4;
  void *local_c0;
  undefined4 local_bc;
  int local_b8;
  void *local_b4;
  uint local_b0;
  undefined4 local_ac;
  int local_a8;
  void *local_a4;
  uint local_a0;
  undefined4 local_9c;
  void *local_98;
  void *local_94;
  void *local_90;
  void *local_8c;
  void *local_88;
  void *local_84;
  undefined4 local_80;
  void *local_7c;
  undefined4 *local_78;
  undefined4 *local_74;
  undefined4 local_70;
  undefined4 local_6c;
  int local_68;
  undefined4 local_64;
  void *local_60;
  void *local_5c;
  void *local_58;
  void *local_54;
  void *local_50;
  int local_4c;
  int local_48;
  uint local_44;
  uint local_40;
  int local_3c;
  int local_38;
  void *local_34;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_30 [28];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  int local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00569248;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_34 = this;
  FUN_00423520((void *)((int)this + 0x24),0);
  *(int *)((int)local_34 + 0x74) = param_1;
  FUN_00548610((char *)((int)local_34 + 0x34),0x40,(int)param_2);
  *(uint *)((int)local_34 + 0x78) = *(uint *)((int)local_34 + 0x78) | 1;
  *(uint *)((int)local_34 + 0x78) = *(uint *)((int)local_34 + 0x78) | 2;
  *(uint *)((int)local_34 + 0x78) = *(uint *)((int)local_34 + 0x78) & 0xfffffff7;
  *(uint *)((int)local_34 + 0x78) = *(uint *)((int)local_34 + 0x78) & 0xffffffef;
  *(uint *)((int)local_34 + 0x78) = *(uint *)((int)local_34 + 0x78) | 0x200;
  FUN_004888c0((int)local_34);
  bVar1 = FUN_00488800(DAT_005c60fc);
  if (CONCAT31(extraout_var,bVar1) == 0) {
    bVar1 = FUN_00488850(0x5ba568);
    local_40 = (uint)bVar1;
    local_44 = local_40;
    iVar2 = FUN_0050fc50(DAT_005c6108);
    local_c0 = (void *)(iVar2 + 0xb08);
    iVar2 = 0xc0;
    puVar6 = param_2;
    pcVar3 = (char *)FUN_00486e10(local_c0,param_1);
    FUN_00548610(pcVar3,iVar2,(int)puVar6);
    iVar2 = FUN_0050fc50(DAT_005c6108);
    local_50 = (void *)(iVar2 + 0xb08);
    local_54 = (void *)FUN_00486e10(local_50,param_1);
    FUN_00488590(local_54,local_44);
    iVar2 = FUN_00488700((int)DAT_005c6108);
    local_58 = (void *)(iVar2 + 0xb08);
    iVar2 = 0xc0;
    puVar6 = param_2;
    pcVar3 = (char *)FUN_00486e10(local_58,param_1);
    FUN_00548610(pcVar3,iVar2,(int)puVar6);
    iVar2 = FUN_00488700((int)DAT_005c6108);
    local_5c = (void *)(iVar2 + 0xb08);
    local_60 = (void *)FUN_00486e10(local_5c,param_1);
    FUN_00488590(local_60,local_44);
  }
  FUN_00488a60(DAT_005c06a4);
  *(uint *)((int)local_34 + 0x78) = *(uint *)((int)local_34 + 0x78) & 0xffffffdf;
  iVar2 = FUN_00478040(0);
  bVar1 = FUN_00478130(iVar2);
  if (bVar1) {
    *(uint *)((int)local_34 + 0x78) = *(uint *)((int)local_34 + 0x78) | 0x20;
  }
  *(undefined4 *)((int)local_34 + 0x8c) = 1;
  *(uint *)((int)local_34 + 0x78) = *(uint *)((int)local_34 + 0x78) & 0xffffffbf;
  local_64 = FUN_00437950(DAT_005c0698);
  uStack_144 = 0x487e62;
  puVar4 = FUN_00450c70(&local_100,(uint *)0x0,0,-1,(undefined4 *)0x0);
  *(undefined4 *)((int)local_34 + 0x14) = *puVar4;
  local_6c = FUN_00470670();
  local_68 = *(int *)((int)local_34 + 0xbc) + 0x16;
  uStack_144 = 0x487eac;
  puVar4 = FUN_00450c70(&local_e4,(uint *)&DAT_0056fe1c,local_68,-1,(undefined4 *)0x0);
  *(undefined4 *)((int)local_34 + 0x18) = *puVar4;
  local_70 = FUN_00437950(DAT_005c0698);
  uStack_144 = 0x487ee2;
  puVar4 = FUN_00450c70(&local_e8,(uint *)0x0,1,-1,(undefined4 *)0x0);
  *(undefined4 *)((int)local_34 + 0x1c) = *puVar4;
  FUN_0040c080(local_30,0x1c);
  FUN_0046a830(local_30,param_2);
  local_8 = 0;
  local_ec = auStack_154;
  iVar2 = FUN_0044ced0((undefined4 *)((int)local_34 + 0x18));
  local_78 = FUN_00486b70(local_124,local_34,local_30,iVar2);
  local_8._0_1_ = 1;
  local_74 = local_78;
  FUN_00486800(auStack_154,local_78);
  FUN_00470180(DAT_005c0698);
  local_8 = (uint)local_8._1_3_ << 8;
  FUN_0046ade0((int)local_124);
  FUN_00426d70(&DAT_005ba830,0x21,0);
  local_7c = (void *)FUN_0041cad0(*(int *)((int)local_34 + 0xc0));
  local_80 = FUN_0045d100(local_7c,0);
  uStack_144 = 0x487fbc;
  puVar4 = FUN_00450c70(&local_f0,(uint *)"effect",6,-1,(undefined4 *)0x0);
  *(undefined4 *)((int)local_34 + 0x20) = *puVar4;
  local_84 = (void *)FUN_00412730(*(int *)((int)local_34 + 0xc0));
  iVar2 = FUN_00485660(local_84,0);
  if (iVar2 == 0) {
    FUN_00422dd0(local_e0 + 5,0,0,0);
    *(int *)((int)local_34 + 0xac) = local_e0[5];
    *(undefined4 *)((int)local_34 + 0xb0) = local_c8;
    *(undefined4 *)((int)local_34 + 0xb4) = local_c4;
  }
  else {
    local_88 = (void *)FUN_00412730(*(int *)((int)local_34 + 0xc0));
    iVar2 = FUN_00485660(local_88,0);
    puVar4 = (undefined4 *)FUN_0047a2c0(iVar2);
    *(undefined4 *)((int)local_34 + 0xac) = *puVar4;
    *(undefined4 *)((int)local_34 + 0xb0) = puVar4[1];
    *(undefined4 *)((int)local_34 + 0xb4) = puVar4[2];
  }
  local_8c = (void *)((int)local_34 + 0x20);
  FUN_004502c0(local_8c,(undefined4 *)((int)local_34 + 0xac));
  local_90 = (void *)((int)local_34 + 0x20);
  iVar2 = FUN_0044c670(local_90,4,0);
  iVar2 = FUN_0045d0c0(iVar2);
  *(undefined4 *)(iVar2 + 8) = param_3;
  local_94 = (void *)((int)local_34 + 0x20);
  iVar2 = FUN_0044c670(local_94,5,0);
  iVar2 = FUN_0045d0c0(iVar2);
  *(undefined4 *)(iVar2 + 8) = param_3;
  *(undefined4 *)((int)local_34 + 0x84) = param_3;
  local_e0[0] = 500000;
  local_e0[1] = 1000000;
  local_e0[2] = 1500000;
  local_e0[3] = 2000000;
  local_e0[4] = 1000000;
  iVar2 = FUN_00474d60(0x5ba568);
  iVar5 = FUN_004640a0(0x5ba568);
  local_48 = iVar2 * local_e0[iVar5];
  *(int *)((int)local_34 + 0x7c) = local_48;
  *(int *)((int)local_34 + 0x80) = local_48;
  if (999999999 < *(int *)((int)local_34 + 0x80)) {
    *(undefined4 *)((int)local_34 + 0x80) = 999999999;
  }
  local_98 = (void *)FUN_0041cad0(*(int *)((int)local_34 + 0xc0));
  local_9c = FUN_0045d100(local_98,0);
  uStack_144 = 0x4881b4;
  FUN_00450c70(&local_f4,(uint *)"effect",0xd,-1,(undefined4 *)0x0);
  local_38 = param_4;
  if ((param_1 < 0x58) || (0x5c < param_1)) {
    local_4c = 0;
  }
  else {
    local_4c = 1;
  }
  local_3c = local_4c;
  if (-1 < *(int *)(DAT_005c6110 + 100 + param_4 * 0x34 + local_4c * 0xc)) {
    local_a4 = (void *)FUN_00478060(0);
    local_a0 = *(uint *)(DAT_005c6110 + 100 + local_38 * 0x34 + local_3c * 0xc);
    local_ac = FUN_004aae10(local_a4,local_a0);
    local_a8 = *(int *)(DAT_005c6110 + local_38 * 0x34 + local_3c * 0xc + 0x68);
    uStack_144 = 0x488293;
    puVar4 = FUN_00450c70(&local_f8,(uint *)0x0,local_a8,-1,(undefined4 *)0x0);
    *(undefined4 *)((int)local_34 + 0x10) = *puVar4;
  }
  FUN_00488960(local_34,*(uint *)(DAT_005c6110 + local_38 * 0x34 + local_3c * 0xc + 0x6c));
  if (*(int *)(DAT_005c6110 + param_4 * 0x34 + 0x7c) != -1) {
    local_b4 = (void *)FUN_00478060(0);
    local_b0 = *(uint *)(DAT_005c6110 + param_4 * 0x34 + 0x7c);
    local_bc = FUN_004aae10(local_b4,local_b0);
    local_b8 = *(int *)(DAT_005c6110 + param_4 * 0x34 + 0x80);
    uStack_144 = 0x48836b;
    FUN_00450c70(&local_fc,(uint *)0x0,local_b8,-1,(undefined4 *)0x0);
  }
  local_8 = 0xffffffff;
  FUN_00449240(local_30);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

