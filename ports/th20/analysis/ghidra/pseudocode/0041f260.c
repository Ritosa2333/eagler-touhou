/* Entry: 0x0041f260; symbol: FUN_0041f260; body bytes: 838 */

void __cdecl FUN_0041f260(undefined1 (*param_1) [32],undefined1 (*param_2) [32],uint param_3)

{
  int iVar1;
  byte bVar2;
  undefined1 auVar3 [32];
  undefined1 auVar4 [16];
  undefined1 auVar5 [16];
  uint uVar6;
  ushort uVar7;
  uint uStack_184;
  short local_e0;
  short sStack_de;
  short sStack_dc;
  short sStack_da;
  short local_a0;
  short sStack_9e;
  short sStack_9c;
  short sStack_9a;
  short sStack_98;
  short sStack_96;
  short sStack_94;
  short sStack_92;
  short local_70;
  short sStack_6e;
  short sStack_6c;
  short sStack_6a;
  short local_60;
  short sStack_5e;
  short sStack_5c;
  short sStack_5a;
  short sStack_58;
  short sStack_56;
  short sStack_54;
  short sStack_52;
  undefined1 (*local_38) [32];
  undefined1 (*local_34) [32];
  uint local_2c;
  
  local_2c = 0;
  local_34 = param_1;
  local_38 = param_2;
  if (DAT_005c0094 != 0) {
    for (; local_2c + 0x10 <= param_3; local_2c = local_2c + 0x10) {
      auVar3 = vpcmpeqw_avx2(*local_34,*local_38);
      uStack_184 = auVar3._28_4_;
      uVar6 = (uint)(SUB321(auVar3 >> 7,0) & 1) | (uint)(SUB321(auVar3 >> 0xf,0) & 1) << 1 |
              (uint)(SUB321(auVar3 >> 0x17,0) & 1) << 2 | (uint)(SUB321(auVar3 >> 0x1f,0) & 1) << 3
              | (uint)(SUB321(auVar3 >> 0x27,0) & 1) << 4 |
              (uint)(SUB321(auVar3 >> 0x2f,0) & 1) << 5 | (uint)(SUB321(auVar3 >> 0x37,0) & 1) << 6
              | (uint)(SUB321(auVar3 >> 0x3f,0) & 1) << 7 |
              (uint)(SUB321(auVar3 >> 0x47,0) & 1) << 8 | (uint)(SUB321(auVar3 >> 0x4f,0) & 1) << 9
              | (uint)(SUB321(auVar3 >> 0x57,0) & 1) << 10 |
              (uint)(SUB321(auVar3 >> 0x5f,0) & 1) << 0xb |
              (uint)(SUB321(auVar3 >> 0x67,0) & 1) << 0xc |
              (uint)(SUB321(auVar3 >> 0x6f,0) & 1) << 0xd |
              (uint)(SUB321(auVar3 >> 0x77,0) & 1) << 0xe | (uint)SUB321(auVar3 >> 0x7f,0) << 0xf |
              (uint)(SUB321(auVar3 >> 0x87,0) & 1) << 0x10 |
              (uint)(SUB321(auVar3 >> 0x8f,0) & 1) << 0x11 |
              (uint)(SUB321(auVar3 >> 0x97,0) & 1) << 0x12 |
              (uint)(SUB321(auVar3 >> 0x9f,0) & 1) << 0x13 |
              (uint)(SUB321(auVar3 >> 0xa7,0) & 1) << 0x14 |
              (uint)(SUB321(auVar3 >> 0xaf,0) & 1) << 0x15 |
              (uint)(SUB321(auVar3 >> 0xb7,0) & 1) << 0x16 | (uint)SUB321(auVar3 >> 0xbf,0) << 0x17
              | (uint)(SUB321(auVar3 >> 199,0) & 1) << 0x18 |
              (uint)(SUB321(auVar3 >> 0xcf,0) & 1) << 0x19 |
              (uint)(SUB321(auVar3 >> 0xd7,0) & 1) << 0x1a |
              (uint)(SUB321(auVar3 >> 0xdf,0) & 1) << 0x1b |
              (uint)(SUB321(auVar3 >> 0xe7,0) & 1) << 0x1c |
              (uint)(SUB321(auVar3 >> 0xef,0) & 1) << 0x1d |
              (uint)(SUB321(auVar3 >> 0xf7,0) & 1) << 0x1e | uStack_184 & 0x80000000;
      if (uVar6 != 0xffffffff) {
        uVar6 = ~uVar6;
        iVar1 = 0;
        if (uVar6 != 0) {
          for (; (uVar6 >> iVar1 & 1) == 0; iVar1 = iVar1 + 1) {
          }
        }
        goto LAB_0041f595;
      }
      local_34 = local_34 + 1;
      local_38 = local_38 + 1;
    }
  }
  for (; local_2c + 8 <= param_3; local_2c = local_2c + 8) {
    local_60 = (short)*(undefined4 *)*local_34;
    sStack_5e = (short)((uint)*(undefined4 *)*local_34 >> 0x10);
    sStack_5c = (short)*(undefined4 *)(*local_34 + 4);
    sStack_5a = (short)((uint)*(undefined4 *)(*local_34 + 4) >> 0x10);
    sStack_58 = (short)*(undefined4 *)(*local_34 + 8);
    sStack_56 = (short)((uint)*(undefined4 *)(*local_34 + 8) >> 0x10);
    sStack_54 = (short)*(undefined4 *)(*local_34 + 0xc);
    sStack_52 = (short)((uint)*(undefined4 *)(*local_34 + 0xc) >> 0x10);
    local_a0 = (short)*(undefined4 *)*local_38;
    sStack_9e = (short)((uint)*(undefined4 *)*local_38 >> 0x10);
    sStack_9c = (short)*(undefined4 *)(*local_38 + 4);
    sStack_9a = (short)((uint)*(undefined4 *)(*local_38 + 4) >> 0x10);
    sStack_98 = (short)*(undefined4 *)(*local_38 + 8);
    sStack_96 = (short)((uint)*(undefined4 *)(*local_38 + 8) >> 0x10);
    sStack_94 = (short)*(undefined4 *)(*local_38 + 0xc);
    sStack_92 = (short)((uint)*(undefined4 *)(*local_38 + 0xc) >> 0x10);
    uVar7 = -(ushort)(sStack_52 == sStack_92);
    auVar5._2_2_ = -(ushort)(sStack_5e == sStack_9e);
    auVar5._0_2_ = -(ushort)(local_60 == local_a0);
    auVar5._4_2_ = -(ushort)(sStack_5c == sStack_9c);
    auVar5._6_2_ = -(ushort)(sStack_5a == sStack_9a);
    auVar5._8_2_ = -(ushort)(sStack_58 == sStack_98);
    auVar5._10_2_ = -(ushort)(sStack_56 == sStack_96);
    auVar5._12_2_ = -(ushort)(sStack_54 == sStack_94);
    auVar5._14_2_ = uVar7;
    uVar7 = (ushort)(SUB161(auVar5 >> 7,0) & 1) | (ushort)(SUB161(auVar5 >> 0xf,0) & 1) << 1 |
            (ushort)(SUB161(auVar5 >> 0x17,0) & 1) << 2 |
            (ushort)(SUB161(auVar5 >> 0x1f,0) & 1) << 3 |
            (ushort)(SUB161(auVar5 >> 0x27,0) & 1) << 4 |
            (ushort)(SUB161(auVar5 >> 0x2f,0) & 1) << 5 |
            (ushort)(SUB161(auVar5 >> 0x37,0) & 1) << 6 |
            (ushort)(SUB161(auVar5 >> 0x3f,0) & 1) << 7 |
            (ushort)(SUB161(auVar5 >> 0x47,0) & 1) << 8 |
            (ushort)(SUB161(auVar5 >> 0x4f,0) & 1) << 9 |
            (ushort)(SUB161(auVar5 >> 0x57,0) & 1) << 10 |
            (ushort)(SUB161(auVar5 >> 0x5f,0) & 1) << 0xb |
            (ushort)(SUB161(auVar5 >> 0x67,0) & 1) << 0xc |
            (ushort)(SUB161(auVar5 >> 0x6f,0) & 1) << 0xd | (ushort)((byte)(uVar7 >> 7) & 1) << 0xe
            | uVar7 & 0x8000;
    if (uVar7 != 0xffff) {
      uVar6 = ~(uint)uVar7;
      iVar1 = 0;
      if (uVar6 != 0) {
        for (; (uVar6 >> iVar1 & 1) == 0; iVar1 = iVar1 + 1) {
        }
      }
      goto LAB_0041f595;
    }
    local_34 = (undefined1 (*) [32])(*local_34 + 0x10);
    local_38 = (undefined1 (*) [32])(*local_38 + 0x10);
  }
  if (local_2c + 4 <= param_3) {
    local_70 = (short)*(undefined4 *)*local_34;
    sStack_6e = (short)((uint)*(undefined4 *)*local_34 >> 0x10);
    sStack_6c = (short)*(undefined4 *)(*local_34 + 4);
    sStack_6a = (short)((uint)*(undefined4 *)(*local_34 + 4) >> 0x10);
    local_e0 = (short)*(undefined4 *)*local_38;
    sStack_de = (short)((uint)*(undefined4 *)*local_38 >> 0x10);
    sStack_dc = (short)*(undefined4 *)(*local_38 + 4);
    sStack_da = (short)((uint)*(undefined4 *)(*local_38 + 4) >> 0x10);
    auVar4._2_2_ = -(ushort)(sStack_6e == sStack_de);
    auVar4._0_2_ = -(ushort)(local_70 == local_e0);
    auVar4._4_2_ = -(ushort)(sStack_6c == sStack_dc);
    auVar4._6_2_ = -(ushort)(sStack_6a == sStack_da);
    auVar4._8_2_ = 0xffff;
    auVar4._10_2_ = 0xffff;
    auVar4._12_2_ = 0xffff;
    auVar4._14_2_ = 0xffff;
    bVar2 = SUB161(auVar4 >> 7,0) & 1 | (SUB161(auVar4 >> 0xf,0) & 1) << 1 |
            (SUB161(auVar4 >> 0x17,0) & 1) << 2 | (SUB161(auVar4 >> 0x1f,0) & 1) << 3 |
            (SUB161(auVar4 >> 0x27,0) & 1) << 4 | (SUB161(auVar4 >> 0x2f,0) & 1) << 5 |
            (SUB161(auVar4 >> 0x37,0) & 1) << 6 | SUB161(auVar4 >> 0x3f,0) << 7;
    if (bVar2 != 0xff) {
      uVar6 = ~(uint)bVar2;
      iVar1 = 0;
      if (uVar6 != 0) {
        for (; (uVar6 >> iVar1 & 1) == 0; iVar1 = iVar1 + 1) {
        }
      }
      goto LAB_0041f595;
    }
    local_2c = local_2c + 4;
  }
  for (; (local_2c < param_3 &&
         (*(short *)(*param_1 + local_2c * 2) == *(short *)(*param_2 + local_2c * 2)));
      local_2c = local_2c + 1) {
  }
LAB_0041f595:
  FUN_005429ee(DAT_005b25c0 ^ (uint)&stack0xffffffe0 ^ (uint)&stack0xffffffe0);
  return;
}

