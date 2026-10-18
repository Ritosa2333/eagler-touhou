/* Entry: 0x004299d0; symbol: FUN_004299d0; body bytes: 1106 */

void __fastcall FUN_004299d0(int param_1)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  undefined4 local_8c [9];
  undefined4 local_68;
  undefined4 local_64;
  undefined4 local_60;
  undefined4 local_5c;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  int local_8;
  
  local_8 = param_1;
  *(undefined4 *)(param_1 + 0x50) = DAT_0056c8cc;
  *(undefined4 *)(param_1 + 0x54) = DAT_0056c8cc;
  *(undefined4 *)(param_1 + 0x58) = DAT_0056c8cc;
  *(undefined4 *)(param_1 + 0x5c) = DAT_0056c8cc;
  *(undefined4 *)(param_1 + 0x68) = DAT_0056c8cc;
  *(undefined4 *)(param_1 + 0x6c) = DAT_0056c8cc;
  *(undefined4 *)(param_1 + 0x490) = 0xffffffff;
  puVar2 = &DAT_0056e068;
  puVar3 = (undefined4 *)(param_1 + 0x3b8);
  for (iVar1 = 0x10; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
  }
  local_8c[0] = 0;
  local_8c[1] = 0;
  local_8c[2] = 0;
  local_8c[3] = 0;
  local_8c[4] = 0;
  local_8c[5] = 0;
  local_8c[6] = 0;
  local_8c[7] = 0;
  puVar2 = local_8c;
  puVar3 = (undefined4 *)(param_1 + 0x498);
  for (iVar1 = 8; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
  }
  *(byte *)(param_1 + 0x49a) = *(byte *)(param_1 + 0x49a) | 1;
  *(uint *)(param_1 + 0x49c) = *(uint *)(param_1 + 0x49c) | 1;
  *(byte *)(param_1 + 0x4a0) = *(byte *)(param_1 + 0x4a0) & 0xfc | 1;
  FUN_00423f50((undefined4 *)(param_1 + 0x4c8));
  FUN_00423f50((undefined4 *)(local_8 + 0x4d8));
  *(undefined4 *)(local_8 + 0x478) = DAT_0056c8cc;
  *(undefined4 *)(local_8 + 0x47c) = DAT_0056e0f0;
  *(undefined4 *)(local_8 + 0x480) = 0x10000;
  *(undefined4 *)(local_8 + 0x558) = 0;
  *(undefined4 *)(local_8 + 0x55c) = 0;
  FUN_00422dd0(&local_3c,0,0,0);
  *(undefined4 *)(local_8 + 0x2c) = local_3c;
  *(undefined4 *)(local_8 + 0x30) = local_38;
  *(undefined4 *)(local_8 + 0x34) = local_34;
  FUN_00422dd0(&local_48,0,0,0);
  *(undefined4 *)(local_8 + 0x484) = local_48;
  *(undefined4 *)(local_8 + 0x488) = local_44;
  *(undefined4 *)(local_8 + 0x48c) = local_40;
  FUN_00422dd0(&local_54,0,0,0);
  *(undefined4 *)(local_8 + 0x38) = local_54;
  *(undefined4 *)(local_8 + 0x3c) = local_50;
  *(undefined4 *)(local_8 + 0x40) = local_4c;
  FUN_00422dd0(&local_60,0,0,0);
  *(undefined4 *)(local_8 + 0x44) = local_60;
  *(undefined4 *)(local_8 + 0x48) = local_5c;
  *(undefined4 *)(local_8 + 0x4c) = local_58;
  FUN_00429250(&local_10,DAT_0056c8cc,DAT_0056c8cc);
  *(undefined4 *)(local_8 + 0x50) = local_10;
  *(undefined4 *)(local_8 + 0x54) = local_c;
  FUN_00429250(&local_18,DAT_0056c8cc,DAT_0056c8cc);
  *(undefined4 *)(local_8 + 0x58) = local_18;
  *(undefined4 *)(local_8 + 0x5c) = local_14;
  FUN_00429250(&local_20,0,0);
  *(undefined4 *)(local_8 + 0x60) = local_20;
  *(undefined4 *)(local_8 + 100) = local_1c;
  FUN_00429250(&local_28,DAT_0056c8cc,DAT_0056c8cc);
  *(undefined4 *)(local_8 + 0x68) = local_28;
  *(undefined4 *)(local_8 + 0x6c) = local_24;
  FUN_00429250(&local_30,0,0);
  *(undefined4 *)(local_8 + 0x70) = local_30;
  *(undefined4 *)(local_8 + 0x74) = local_2c;
  *(undefined4 *)(local_8 + 0x7c) = 0;
  *(undefined4 *)(local_8 + 0x78) = 0;
  FUN_00422dd0(local_8c + 8,0,0,0);
  *(undefined4 *)(local_8 + 0x80) = local_8c[8];
  *(undefined4 *)(local_8 + 0x84) = local_68;
  *(undefined4 *)(local_8 + 0x88) = local_64;
  FUN_00429990(local_8 + 0x8c);
  FUN_00429990(local_8 + 0xe0);
  FUN_00429970(local_8 + 0x134);
  FUN_00429990(local_8 + 0x160);
  FUN_00429970(local_8 + 0x1b4);
  FUN_004299b0(local_8 + 0x1e0);
  FUN_004299b0(local_8 + 0x220);
  FUN_004299b0(local_8 + 0x260);
  FUN_00429990(local_8 + 0x2a0);
  FUN_00429970(local_8 + 0x2f4);
  FUN_00429970(local_8 + 800);
  FUN_00429970(local_8 + 0x34c);
  *(undefined4 *)(local_8 + 0x570) = 0;
  return;
}

