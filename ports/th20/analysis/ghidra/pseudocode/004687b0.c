/* Entry: 0x004687b0; symbol: FUN_004687b0; body bytes: 1250 */

void __cdecl FUN_004687b0(uint *param_1,uint *param_2,float param_3,int param_4)

{
  int iVar1;
  int *piVar2;
  undefined4 *puVar3;
  size_t *psVar4;
  int *unaff_FS_OFFSET;
  undefined1 local_130 [12];
  undefined4 local_124;
  undefined8 local_120;
  int local_118;
  uint *local_114;
  undefined4 local_110;
  uint *local_10c;
  undefined4 local_108;
  uint *local_104;
  undefined4 local_100;
  undefined8 local_fc;
  int local_f4;
  undefined4 local_f0;
  uint *local_ec;
  undefined4 local_e8;
  uint *local_e4;
  float local_e0;
  int local_dc;
  int local_d8;
  char *local_d4;
  float local_d0;
  int local_cc;
  int local_c8;
  uint *local_c4;
  uint local_c0;
  uint *local_bc;
  uint local_b8;
  int local_b4;
  size_t local_b0;
  char local_a9;
  uint *local_a8;
  char local_a2;
  char local_a1;
  undefined *local_a0;
  undefined *local_9c;
  int local_98 [3];
  uint local_8c [30];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00568c90;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_98[2] = FUN_0040c990(&param_3);
  if (local_98[2] == 0) {
    if (param_1 == param_2) {
      local_ec = param_2;
      local_e8 = 0x84;
    }
    else {
      *(undefined1 *)param_1 = 0x30;
      local_f4 = (int)param_1 + 1;
      local_f0 = 0;
    }
  }
  else {
    if (param_4 < 0) {
      param_4 = 6;
    }
    else if (param_4 == 0) {
      param_4 = 1;
    }
    else if (999999 < param_4) {
      param_4 = 1000000;
    }
    if (param_4 < 8) {
      iVar1 = (((param_4 + -1) * (param_4 + 10)) / 2) * 4;
      local_9c = &DAT_00595e58 + iVar1;
      local_a0 = &UNK_00595e6c + param_4 * 4 + iVar1;
    }
    else {
      local_9c = &DAT_00595f58;
      piVar2 = std::_Min_value<int>(&param_4,(int *)&DAT_0056f578);
      local_a0 = local_9c + *piVar2 * 4 + 0x14;
    }
    puVar3 = FUN_0046a570(local_130,&local_9c,&local_a0,local_98 + 2);
    local_cc = FUN_0046b2e0(puVar3);
    local_d8 = local_cc - (int)local_9c >> 2;
    local_b4 = local_d8 + -5;
    if ((local_b4 < param_4) && (-5 < local_b4)) {
      local_a2 = '\x01';
    }
    else {
      local_a2 = '\0';
    }
    local_a1 = local_a2;
    local_124 = 0x75;
    local_98[1] = 0x25;
    local_98[0] = 0x6f;
    local_c4 = local_8c;
    local_a8 = (uint *)0x0;
    local_bc = (uint *)0x0;
    local_c8 = 0;
    if (local_a2 == '\0') {
      local_dc = param_4 + -1;
      psVar4 = (size_t *)std::_Min_value<int>(&local_dc,local_98);
      local_b0 = *psVar4;
      local_e0 = param_3;
      local_e4 = (uint *)FUN_0046a220((int)local_8c);
      local_fc = FUN_00468f40(local_8c,local_e4,local_e0,local_b0);
      local_a9 = 'e';
      local_bc = (uint *)FUN_0046a3b0((char *)local_8c,(int)local_fc,&local_a9);
      local_c8 = (int)local_fc;
      local_a8 = local_bc;
    }
    else {
      local_118 = param_4 - (local_d8 + -4);
      psVar4 = (size_t *)std::_Min_value<int>(&local_118,local_98 + 1);
      local_b0 = *psVar4;
      local_d0 = param_3;
      local_d4 = (char *)FUN_0046a220((int)local_8c);
      local_120 = FUN_00468700((char *)local_8c,local_d4,local_d0,local_b0);
      local_a8 = (uint *)local_120;
    }
    if (0 < (int)local_b0) {
      for (; *(char *)((int)local_a8 + -1) == '0'; local_a8 = (uint *)((int)local_a8 + -1)) {
      }
      if (*(char *)((int)local_a8 + -1) == '.') {
        local_a8 = (uint *)((int)local_a8 + -1);
      }
    }
    local_b8 = (int)local_a8 - (int)local_c4;
    if ((int)param_2 - (int)param_1 < (int)local_b8) {
      local_104 = param_2;
      local_100 = 0x84;
    }
    else {
      FUN_00543e20(param_1,local_c4,local_b8);
      param_1 = (uint *)((int)param_1 + local_b8);
      if (local_a1 == '\0') {
        local_c0 = local_c8 - (int)local_bc;
        if ((int)param_2 - (int)param_1 < (int)local_c0) {
          local_10c = param_2;
          local_108 = 0x84;
          goto LAB_00468c79;
        }
        FUN_00543e20(param_1,local_bc,local_c0);
        param_1 = (uint *)((int)param_1 + local_c0);
      }
      local_114 = param_1;
      local_110 = 0;
    }
  }
LAB_00468c79:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

