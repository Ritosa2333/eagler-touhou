/* Entry: 0x00421b00; symbol: FUN_00421b00; body bytes: 3490 */

void __fastcall FUN_00421b00(int *param_1)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  short *psVar4;
  int *unaff_FS_OFFSET;
  uint local_1c4;
  uint local_1c0;
  uint local_1bc;
  uint local_1b8;
  uint local_1b4;
  uint local_1b0;
  uint local_1ac;
  uint local_1a8;
  uint local_1a4;
  uint local_1a0;
  uint local_19c;
  uint local_198;
  uint local_194;
  uint local_190;
  uint local_18c;
  uint local_188;
  uint local_184;
  uint local_180;
  uint local_17c;
  uint local_178;
  uint local_174;
  uint local_170;
  uint local_16c;
  uint local_168;
  uint local_164;
  uint local_160;
  int local_15c;
  int local_150;
  int local_14c;
  uint local_144;
  int local_13c;
  int local_138;
  char local_10c [224];
  undefined4 local_2c;
  undefined1 local_28 [4];
  ushort local_24;
  byte local_22;
  byte local_21;
  short local_20;
  short local_1e;
  uint local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056810d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_18 = 0;
  _memset(param_1 + 0xb4,0,0x100);
  uVar2 = FUN_0040c310(&DAT_005c0240,0xf);
  FUN_0040b730(&local_2c,uVar2);
  local_8 = 0;
  iVar3 = FUN_00421990(DAT_005b8898,1);
  local_1c0 = (uint)(iVar3 == param_1[1]);
  psVar4 = (short *)FUN_004217f0(DAT_005b8898,local_1c0);
  uVar1 = local_18;
  iVar3 = *param_1;
  if (iVar3 == 0) {
    if (DAT_005b67c0 != '\0') {
      GetKeyboardState((PBYTE)(param_1 + 0xb4));
      if (local_18 != 0) {
        FUN_0041de90(DAT_005b8898,*param_1);
      }
      if (*param_1 == 0) {
        if ((*(byte *)((int)param_1 + psVar4[0x14] + 0x2d0) & 0x80) == 0) {
          local_1c4 = 0;
        }
        else {
          local_1c4 = 0x10;
        }
        if ((*(byte *)((int)param_1 + psVar4[0x15] + 0x2d0) & 0x80) == 0) {
          local_160 = 0;
        }
        else {
          local_160 = 0x20;
        }
        if ((*(byte *)((int)param_1 + psVar4[0x16] + 0x2d0) & 0x80) == 0) {
          local_164 = 0;
        }
        else {
          local_164 = 0x40;
        }
        if ((*(byte *)((int)param_1 + psVar4[0x17] + 0x2d0) & 0x80) == 0) {
          local_168 = 0;
        }
        else {
          local_168 = 0x80;
        }
        local_16c = (uint)((*(byte *)((int)param_1 + psVar4[0x10] + 0x2d0) & 0x80) != 0);
        if ((*(byte *)((int)param_1 + psVar4[0x11] + 0x2d0) & 0x80) == 0) {
          local_170 = 0;
        }
        else {
          local_170 = 4;
        }
        if ((*(byte *)((int)param_1 + psVar4[0x12] + 0x2d0) & 0x80) == 0) {
          local_174 = 0;
        }
        else {
          local_174 = 8;
        }
        if ((*(byte *)((int)param_1 + psVar4[0x13] + 0x2d0) & 0x80) == 0) {
          local_178 = 0;
        }
        else {
          local_178 = 0x100;
        }
        if ((*(byte *)((int)param_1 + 0x2dd) & 0x80) == 0) {
          local_17c = 0;
        }
        else {
          local_17c = 0x80000;
        }
        if ((*(byte *)(param_1 + 0xbd) & 0x80) == 0) {
          local_180 = 0;
        }
        else {
          local_180 = 0x40000;
        }
        if ((*(byte *)(param_1 + 200) & 0x80) == 0) {
          local_184 = 0;
        }
        else {
          local_184 = 0x40000;
        }
        if ((*(byte *)((int)param_1 + 0x2f3) & 0x80) == 0) {
          local_188 = 0;
        }
        else {
          local_188 = 0x200000;
        }
        if ((*(byte *)((int)param_1 + 0x322) & 0x80) == 0) {
          local_18c = 0;
        }
        else {
          local_18c = 0x200000;
        }
        if ((*(byte *)(param_1 + 0xce) & 0x80) == 0) {
          local_190 = 0;
        }
        else {
          local_190 = 0x10;
        }
        if ((*(byte *)((int)param_1 + 0x332) & 0x80) == 0) {
          local_194 = 0;
        }
        else {
          local_194 = 0x20;
        }
        if ((*(byte *)(param_1 + 0xcd) & 0x80) == 0) {
          local_198 = 0;
        }
        else {
          local_198 = 0x40;
        }
        if ((*(byte *)((int)param_1 + 0x336) & 0x80) == 0) {
          local_19c = 0;
        }
        else {
          local_19c = 0x80;
        }
        if ((*(byte *)((int)param_1 + 0x337) & 0x80) == 0) {
          local_1a0 = 0;
        }
        else {
          local_1a0 = 0x50;
        }
        if ((*(byte *)((int)param_1 + 0x339) & 0x80) == 0) {
          local_1a4 = 0;
        }
        else {
          local_1a4 = 0x90;
        }
        if ((*(byte *)((int)param_1 + 0x331) & 0x80) == 0) {
          local_1a8 = 0;
        }
        else {
          local_1a8 = 0x60;
        }
        if ((*(byte *)((int)param_1 + 0x333) & 0x80) == 0) {
          local_1ac = 0;
        }
        else {
          local_1ac = 0xa0;
        }
        local_18 = local_18 | local_1c4 | local_160 | local_164 | local_168 | local_16c | local_170
                   | local_174 | local_178 | local_17c | local_180 | local_184 | local_188 |
                   local_18c | local_190 | local_194 | local_198 | local_19c | local_1a0 | local_1a4
                   | local_1a8 | local_1ac;
      }
    }
  }
  else if (iVar3 == 1) {
    iVar3 = (**(code **)(*(int *)param_1[2] + 100))(param_1[2]);
    if (iVar3 < 0) {
      local_15c = 0;
      local_14c = (**(code **)(*(int *)param_1[2] + 0x1c))(param_1[2]);
      do {
        if (local_14c != -0x7ff8ffe2) {
          local_8 = 0xffffffff;
          FUN_0040b900(&local_2c);
          goto LAB_00422889;
        }
        local_14c = (**(code **)(*(int *)param_1[2] + 0x1c))(param_1[2]);
        local_15c = local_15c + 1;
      } while (local_15c < 400);
      local_8 = 0xffffffff;
      FUN_0040b900(&local_2c);
      goto LAB_00422889;
    }
    _memset(&local_13c,0,0x110);
    iVar3 = (**(code **)(*(int *)param_1[2] + 0x24))(param_1[2],0x110,&local_13c);
    if (iVar3 < 0) {
      local_8 = 0xffffffff;
      FUN_0040b900(&local_2c);
      goto LAB_00422889;
    }
    FUN_00420510(&local_18,*psVar4,1,(int)local_10c);
    FUN_00420510(&local_18,psVar4[1],4,(int)local_10c);
    FUN_00420510(&local_18,psVar4[3],0x100,(int)local_10c);
    FUN_00420510(&local_18,psVar4[2],8,(int)local_10c);
    if (DAT_005c4f78 < local_13c) {
      local_1b0 = 0x80;
    }
    else {
      local_1b0 = 0;
    }
    if (SBORROW4(local_13c,-(int)DAT_005c4f78) == local_13c + DAT_005c4f78 < 0) {
      local_1b4 = 0;
    }
    else {
      local_1b4 = 0x40;
    }
    if (DAT_005c4f7a < local_138) {
      local_1b8 = 0x20;
    }
    else {
      local_1b8 = 0;
    }
    if (SBORROW4(local_138,-(int)DAT_005c4f7a) == local_138 + DAT_005c4f7a < 0) {
      local_1bc = 0;
    }
    else {
      local_1bc = 0x10;
    }
    local_18 = local_18 | local_1b0 | local_1b4 | local_1b8 | local_1bc;
    for (local_150 = 0; local_150 < 0x20; local_150 = local_150 + 1) {
      if (local_10c[local_150] != '\0') {
        *(undefined1 *)((int)param_1 + local_150 + 0x2d0) = 0x80;
      }
    }
    if (uVar1 != local_18) {
      FUN_0041de90(DAT_005b8898,*param_1);
    }
  }
  else if (iVar3 == 2) {
    _memset(local_28,0,0x10);
    iVar3 = XInputGetState(param_1[3],local_28);
    if (iVar3 == 0) {
      local_144 = (uint)local_24;
      if (0x1d < local_22) {
        local_144 = local_144 | 0x10000;
      }
      if (0x1d < local_21) {
        local_144 = local_144 | 0x20000;
      }
      if (0x1ea8 < local_1e) {
        local_144 = local_144 | 1;
      }
      if (local_1e < -0x1ea8) {
        local_144 = local_144 | 2;
      }
      if (0x1ea8 < local_20) {
        local_144 = local_144 | 8;
      }
      if (local_20 < -0x1ea8) {
        local_144 = local_144 | 4;
      }
      if ((local_144 & 1) != 0) {
        local_18 = local_18 | 0x10;
      }
      if ((local_144 & 2) != 0) {
        local_18 = local_18 | 0x20;
      }
      if ((local_144 & 4) != 0) {
        local_18 = local_18 | 0x40;
      }
      if ((local_144 & 8) != 0) {
        local_18 = local_18 | 0x80;
      }
      if ((local_144 & *(uint *)(&DAT_005ae220 + psVar4[8] * 4)) != 0) {
        local_18 = local_18 | 1;
      }
      if ((local_144 & *(uint *)(&DAT_005ae220 + psVar4[9] * 4)) != 0) {
        local_18 = local_18 | 4;
      }
      if ((local_144 & *(uint *)(&DAT_005ae220 + psVar4[10] * 4)) != 0) {
        local_18 = local_18 | 8;
      }
      if ((local_144 & *(uint *)(&DAT_005ae220 + psVar4[0xb] * 4)) != 0) {
        local_18 = local_18 | 0x100;
      }
      if ((local_24 & 0x1000) != 0) {
        *(undefined1 *)(param_1 + 0xb4) = 0x80;
      }
      if ((local_24 & 0x2000) != 0) {
        *(undefined1 *)((int)param_1 + 0x2d1) = 0x80;
      }
      if ((local_24 & 0x4000) != 0) {
        *(undefined1 *)((int)param_1 + 0x2d2) = 0x80;
      }
      if ((local_24 & 0x8000) != 0) {
        *(undefined1 *)((int)param_1 + 0x2d3) = 0x80;
      }
      if ((local_24 & 0x40) != 0) {
        *(undefined1 *)(param_1 + 0xb6) = 0x80;
      }
      if ((local_24 & 0x80) != 0) {
        *(undefined1 *)((int)param_1 + 0x2d9) = 0x80;
      }
      if ((local_24 & 0x100) != 0) {
        *(undefined1 *)(param_1 + 0xb5) = 0x80;
      }
      if ((local_24 & 0x200) != 0) {
        *(undefined1 *)((int)param_1 + 0x2d5) = 0x80;
      }
      if ((local_24 & 0x10) != 0) {
        *(undefined1 *)((int)param_1 + 0x2da) = 0x80;
      }
      if ((local_24 & 0x20) != 0) {
        *(undefined1 *)((int)param_1 + 0x2db) = 0x80;
      }
      if (0x1d < local_22) {
        *(undefined1 *)((int)param_1 + 0x2d6) = 0x80;
      }
      if (0x1d < local_21) {
        *(undefined1 *)((int)param_1 + 0x2d7) = 0x80;
      }
      if (local_18 != 0) {
        FUN_0041de90(DAT_005b8898,*param_1);
      }
    }
  }
  param_1[5] = param_1[4];
  param_1[4] = local_18;
  FUN_004228b0((uint *)(param_1 + 4));
  local_8 = 0xffffffff;
  FUN_0040b900(&local_2c);
LAB_00422889:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

