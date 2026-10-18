/* Entry: 0x00417ab0; symbol: FUN_00417ab0; body bytes: 542 */

uint __thiscall FUN_00417ab0(void *this,int param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int local_30;
  int local_2c;
  int local_28;
  int local_1c;
  int local_18;
  ushort *local_10;
  ushort *local_c;
  ushort *local_8;
  
  iVar1 = FUN_004156c0((int)this);
  local_8 = (ushort *)FUN_004156a0((int)this);
  iVar2 = *(int *)((int)this + 0x100);
  if (iVar2 == 0x15) {
    for (local_18 = 0; local_18 < param_1; local_18 = local_18 + 1) {
      iVar2 = FUN_00415800((int)this);
      local_10 = local_8 + iVar2 * local_18 * 2 + param_2 * 2;
      for (local_28 = 0; local_28 < param_3; local_28 = local_28 + 1) {
        *(byte *)((int)local_10 + 3) = *(byte *)((int)local_10 + 3) ^ 0xff;
        local_10 = local_10 + 2;
      }
    }
  }
  else if (iVar2 == 0x19) {
    for (local_2c = 0; local_18 = local_2c, local_2c < iVar1 * param_1; local_2c = local_2c + 2) {
      *local_8 = *local_8 & 0x7fff | (*local_8 >> 0xf ^ 1) << 0xf;
      if (-1 < (short)*local_8) {
        *local_8 = *local_8 & 0x83ff;
        *local_8 = *local_8 & 0xfc1f;
        *local_8 = *local_8 & 0xffe0;
      }
      local_8 = local_8 + 1;
    }
  }
  else {
    if (iVar2 != 0x1a) {
      return (uint)this & 0xffffff00;
    }
    for (local_1c = 0; local_18 = local_1c, local_1c < param_1; local_1c = local_1c + 1) {
      iVar2 = FUN_00415800((int)this);
      local_c = local_8 + iVar2 * local_1c + param_2;
      for (local_30 = 0; local_30 < param_3; local_30 = local_30 + 1) {
        *local_c = *local_c & 0xfff | (*local_c >> 0xc ^ 0xf) << 0xc;
        local_c = local_c + 1;
      }
    }
  }
  return CONCAT31((int3)((uint)local_18 >> 8),1);
}

