/* Entry: 0x0051feb0; symbol: FUN_0051feb0; body bytes: 1543 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_0051feb0(void *this,int param_1,int param_2,int param_3,int param_4)

{
  bool bVar1;
  int iVar2;
  undefined1 local_134 [12];
  float *local_128;
  void *local_124;
  void *local_120;
  void *local_11c;
  void *local_118;
  void *local_114;
  void *local_110;
  void *local_10c;
  uint local_108;
  uint local_104;
  uint local_100;
  uint local_fc;
  int local_f8;
  int local_f4;
  uint local_f0;
  int local_ec;
  uint local_e8;
  void *local_e4;
  int local_e0;
  int local_dc;
  float local_d8;
  float local_d4;
  char local_cc [196];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_dc = 0;
  for (local_f4 = 0; local_f4 < 5; local_f4 = local_f4 + 1) {
    *(undefined4 *)(param_3 + local_f4 * 4) = 0xfffffffe;
  }
  local_10c = (void *)((int)this + 0x154);
  local_e4 = this;
  bVar1 = FUN_00423560(local_10c,2);
  if (bVar1) {
    for (local_f8 = 0; local_f8 < 7; local_f8 = local_f8 + 1) {
      FUN_0052cdf0(local_e4,local_f8 + 0x7f);
    }
    if (param_1 == 6) {
      FUN_0052cf80(local_e4,0x83);
      FUN_0052cc90(local_e4,0x83);
    }
    else {
      for (local_ec = 0; local_ec < 4; local_ec = local_ec + 1) {
        FUN_0052cf80(local_e4,local_ec + 0x7f);
        FUN_0052cc90(local_e4,local_ec + 0x7f);
      }
    }
  }
  local_e8 = 0;
  local_108 = 0;
  FUN_00422dd0(&local_d8,DAT_00575678,DAT_00571058,0);
  local_e0 = 0;
  while ((local_e0 < 5 &&
         (local_dc = *(int *)(&DAT_005af220 + local_e0 * 4 + param_2 * 0x14 + param_1 * 0x104),
         -1 < local_dc))) {
    if (param_1 == 6) {
      local_fc = (uint)(*(int *)(&DAT_005af058 + local_dc * 4) != 4);
      local_f0 = local_fc;
    }
    else {
      if (*(int *)(&DAT_005af058 + local_dc * 4) < 5) {
        local_100 = *(uint *)(&DAT_005af058 + local_dc * 4);
      }
      else {
        local_100 = 4;
      }
      local_f0 = local_100;
    }
    iVar2 = FUN_00488700((int)DAT_005c6108);
    local_110 = (void *)(iVar2 + 0xb08);
    iVar2 = FUN_00486e10(local_110,local_dc);
    if (*(int *)(iVar2 + 200) == 0) {
      *(undefined4 *)(param_3 + local_f0 * 4) = 0xffffffff;
      FUN_00470ad0(DAT_005c0698,0xff404040);
      FUN_004c6180(DAT_005c0698,4);
      FUN_0040ddd0(&stack0xfffffec0,&DAT_0057555c);
      FUN_0046c210(DAT_005c0698,&local_d8);
    }
    else {
      *(int *)(param_3 + local_f0 * 4) = local_dc;
      iVar2 = FUN_00488700((int)DAT_005c6108);
      local_114 = (void *)(iVar2 + 0xb08);
      iVar2 = FUN_00486e10(local_114,local_dc);
      FUN_00548610(local_cc,0xc1,iVar2);
      for (local_e8 = _strlen(local_cc); (int)local_e8 < 0x2a; local_e8 = local_e8 + 1) {
        local_cc[local_e8] = ' ';
      }
      local_104 = local_e8;
      if (0xc0 < local_e8) {
        ___report_rangecheckfailure();
      }
      local_cc[local_104] = '\0';
      if (param_4 == local_e0) {
        FUN_00470ad0(DAT_005c0698,0xffffff00);
      }
      else {
        FUN_00470ad0(DAT_005c0698,0xff808080);
      }
      iVar2 = FUN_0050fc50(DAT_005c6108);
      local_118 = (void *)(iVar2 + 0xb08);
      iVar2 = FUN_00486e10(local_118,local_dc);
      if (*(int *)(iVar2 + 200) == 0) {
        iVar2 = FUN_0050fc50(DAT_005c6108);
        local_11c = (void *)(iVar2 + 0xb08);
        iVar2 = FUN_00486e10(local_11c,local_dc);
        if (*(int *)(iVar2 + 0xcc) == 0) {
          if (param_4 == local_e0) {
            FUN_00470ad0(DAT_005c0698,0xff808040);
          }
          else {
            FUN_00470ad0(DAT_005c0698,0xff404040);
          }
        }
      }
      FUN_004c6180(DAT_005c0698,4);
      FUN_0040ddd0(&stack0xfffffebc," No.%3d  %s");
      FUN_0046c210(DAT_005c0698,&local_d8);
      if (param_4 == local_e0) {
        iVar2 = FUN_0050fc50(DAT_005c6108);
        local_120 = (void *)(iVar2 + 0xb08);
        iVar2 = FUN_00486e10(local_120,local_dc);
        if (*(int *)(iVar2 + 200) == 0) {
          iVar2 = FUN_0050fc50(DAT_005c6108);
          local_124 = (void *)(iVar2 + 0xb08);
          iVar2 = FUN_00486e10(local_124,local_dc);
          if (*(int *)(iVar2 + 0xcc) == 0) {
            FUN_00470ad0(DAT_005c0698,0xffffffff);
            FUN_004c6180(DAT_005c0698,4);
            local_128 = (float *)FUN_00422dd0(local_134,DAT_0057567c,DAT_00571060,0);
            FUN_0040ddd0(&stack0xfffffec4,&DAT_00575534);
            FUN_0046c210(DAT_005c0698,local_128);
          }
        }
      }
    }
    local_d4 = local_d4 + _DAT_0057565c;
    if ((int)local_108 < (int)local_f0) {
      local_108 = local_f0;
    }
    local_e0 = local_e0 + 1;
  }
  FUN_004e67e0(DAT_005c0698);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

