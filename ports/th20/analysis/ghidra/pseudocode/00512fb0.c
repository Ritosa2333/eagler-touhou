/* Entry: 0x00512fb0; symbol: FUN_00512fb0; body bytes: 1847 */

void __thiscall FUN_00512fb0(void *this,int param_1,int param_2)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined1 auStack_19c [8];
  undefined4 uStack_194;
  undefined1 local_16c [12];
  undefined1 local_160 [12];
  undefined4 local_154;
  undefined4 local_150;
  undefined1 local_14c [4];
  undefined1 *local_148;
  undefined4 local_144 [4];
  float *local_134 [5];
  void *local_120;
  void *local_11c;
  void *local_118;
  void *local_114;
  void *local_110;
  void *local_10c;
  void *local_108;
  void *local_104;
  void *local_100;
  void *local_fc;
  void *local_f8;
  void *local_f4;
  void *local_f0;
  void *local_ec;
  void *local_e8;
  void *local_e4;
  void *local_e0;
  void *local_dc;
  void *local_d8;
  void *local_d4;
  void *local_d0;
  Image *local_cc;
  Image *local_c8;
  Image *local_c4;
  void *local_c0;
  char *local_bc;
  void *local_b8;
  void *local_b4;
  undefined4 local_b0;
  undefined4 local_ac;
  int local_a8;
  undefined4 local_a4;
  int local_a0;
  void *local_9c;
  undefined4 *local_98;
  undefined4 local_94;
  undefined4 local_90;
  undefined4 local_8c;
  undefined4 local_74 [12];
  int local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_28;
  undefined4 local_24;
  undefined1 local_20 [4];
  undefined4 local_1c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *(int *)((int)this + 0x14) = param_1;
  local_9c = this;
  FUN_00423520((void *)((int)this + 0x1c),0);
  *(int *)((int)local_9c + 0x2c) = param_2;
  local_11c = (void *)FUN_00478060(0);
  local_ac = FUN_004aae10(local_11c,2);
  local_134[4] = (float *)FUN_00422dd0(local_16c,0,0,0);
  uStack_194 = 0x513077;
  puVar1 = FUN_004790e0(&local_150,(uint *)"enemy",param_2 + 0x13f,local_134[4],0,-1,
                        (undefined4 *)0x0);
  *(undefined4 *)((int)local_9c + 0x18) = *puVar1;
  FUN_0047ba30(&local_40);
  puVar1 = (undefined4 *)FUN_004aadb0((int *)((int)local_9c + 0x14));
  local_40 = *puVar1;
  local_3c = puVar1[1];
  local_38 = puVar1[2];
  local_28 = 0;
  local_24 = DAT_005742cc;
  local_1c = DAT_0056cd98;
  local_134[0] = (float *)0xffff0000;
  local_134[1] = (float *)0xff0000ff;
  local_134[2] = (float *)0xffffff00;
  local_134[3] = (float *)0xff00ff00;
  FUN_0045d650(local_20,local_134[param_2]);
  local_b0 = FUN_00437520(0);
  FUN_0049db70(&local_154,0xb,&local_40,0);
  local_b4 = (void *)((int)local_9c + 0x18);
  FUN_00450850(local_b4,local_9c);
  local_b8 = (void *)((int)local_9c + 0x18);
  FUN_0044f100(local_b8,FUN_00511640);
  local_98 = (undefined4 *)FUN_004aaac0(&param_1);
  FUN_0047bb30(&local_94);
  puVar1 = FUN_00422dd0(local_160,0,0,0);
  local_94 = *puVar1;
  local_90 = puVar1[1];
  local_8c = puVar1[2];
  if (local_98 != (undefined4 *)0x0) {
    puVar1 = (undefined4 *)FUN_00513d60((int)local_98);
    puVar3 = local_74;
    for (iVar2 = 0xc; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar3 = *puVar1;
      puVar1 = puVar1 + 1;
      puVar3 = puVar3 + 1;
    }
  }
  local_44 = param_1;
  iVar2 = FUN_00464080(0);
  local_144[0] = FUN_005114f0(iVar2);
  iVar2 = FUN_00464080(0);
  local_144[1] = FUN_00511460(iVar2);
  iVar2 = FUN_00464080(0);
  local_144[2] = FUN_00511530(iVar2);
  iVar2 = FUN_00464080(0);
  local_144[3] = FUN_005114a0(iVar2);
  *(undefined4 *)((int)local_9c + 0x30) = local_144[param_2];
  local_c0 = (void *)FUN_00478060(0);
  local_bc = (&PTR_s_StoneAttackLevel_R1_005b09e8)[param_2 * 5 + *(int *)((int)local_9c + 0x30)];
  local_98 = FUN_004a8920(local_c0,local_bc,&local_94,(int)local_98);
  iVar2 = FUN_00464080(0);
  local_a0 = FUN_00513d80(iVar2);
  local_a0 = local_a0 % 3;
  if (local_a0 == 0) {
    local_c4 = (Image *)FUN_00513d40((int)local_98);
    Gdiplus::Image::SetNativeImage(local_c4,(GpImage *)0x6);
  }
  else if (local_a0 == 1) {
    local_c8 = (Image *)FUN_00513d40((int)local_98);
    Gdiplus::Image::SetNativeImage(local_c8,(GpImage *)0x6);
  }
  else if (local_a0 == 2) {
    local_cc = (Image *)FUN_00513d40((int)local_98);
    Gdiplus::Image::SetNativeImage(local_cc,(GpImage *)0x4);
  }
  local_a4 = *(undefined4 *)((int)local_9c + 0x30);
  switch(local_a4) {
  case 0:
    local_d0 = (void *)FUN_00513d40((int)local_98);
    FUN_00514130(local_d0,1,0x1e);
    local_d4 = (void *)FUN_00513d40((int)local_98);
    FUN_00514130(local_d4,2,0x1e);
    local_d8 = (void *)FUN_00513d40((int)local_98);
    FUN_0049c830(local_d8,DAT_0056fa28,DAT_0056fa28);
    break;
  case 1:
    local_dc = (void *)FUN_00513d40((int)local_98);
    FUN_00514130(local_dc,1,0x21);
    local_e0 = (void *)FUN_00513d40((int)local_98);
    FUN_00514130(local_e0,2,0x21);
    local_e4 = (void *)FUN_00513d40((int)local_98);
    FUN_0049c830(local_e4,DAT_0056fa30,DAT_0056fa30);
    break;
  case 2:
    local_e8 = (void *)FUN_00513d40((int)local_98);
    FUN_00514130(local_e8,1,0x24);
    local_ec = (void *)FUN_00513d40((int)local_98);
    FUN_00514130(local_ec,2,0x24);
    local_f0 = (void *)FUN_00513d40((int)local_98);
    FUN_0049c830(local_f0,DAT_005742d4,DAT_005742d4);
    break;
  case 3:
    local_f4 = (void *)FUN_00513d40((int)local_98);
    FUN_00514130(local_f4,1,0x27);
    local_f8 = (void *)FUN_00513d40((int)local_98);
    FUN_00514130(local_f8,2,0x27);
    local_fc = (void *)FUN_00513d40((int)local_98);
    FUN_0049c830(local_fc,DAT_0056fe7c,DAT_0056fe7c);
    break;
  default:
    local_100 = (void *)FUN_00513d40((int)local_98);
    FUN_00514130(local_100,1,0x2a);
    local_104 = (void *)FUN_00513d40((int)local_98);
    FUN_00514130(local_104,2,0x2a);
    local_108 = (void *)FUN_00513d40((int)local_98);
    FUN_0049c830(local_108,DAT_0056f090,DAT_0056f090);
  }
  local_a8 = param_2;
  switch(param_2) {
  case 0:
    local_10c = (void *)FUN_00464080(0);
    FUN_00513b50(local_10c,1);
    break;
  case 1:
    local_110 = (void *)FUN_00464080(0);
    FUN_00513a90(local_110,1);
    break;
  case 2:
    local_114 = (void *)FUN_00464080(0);
    FUN_00513c10(local_114,1);
    break;
  case 3:
    local_118 = (void *)FUN_00464080(0);
    FUN_00513af0(local_118,1);
  }
  local_120 = (void *)FUN_00464080(0);
  FUN_00513bb0(local_120,1);
  local_148 = auStack_19c;
  puVar1 = FUN_004142a0(local_14c,local_9c);
  FUN_005116a0(auStack_19c,puVar1);
  FUN_005140b0((int)local_98);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

