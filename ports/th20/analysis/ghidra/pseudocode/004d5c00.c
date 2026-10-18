/* Entry: 0x004d5c00; symbol: FUN_004d5c00; body bytes: 582 */

void __thiscall FUN_004d5c00(void *this,uint *param_1)

{
  float fVar1;
  float fVar2;
  float local_38;
  int local_34;
  int local_30;
  float *local_2c;
  undefined4 *local_24;
  float local_20 [3];
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_24 = *(undefined4 **)((int)this + 0x6d8);
  local_30 = 0;
  *(uint *)((int)this + 0x1c) = *(uint *)((int)this + 0x1c) | 0x40;
  local_38 = 0.0;
  local_2c = *(float **)((int)this + 0x1328);
  *(undefined4 *)((int)this + 0x84) = DAT_0056c8cc;
  for (local_34 = 0; local_34 < *(int *)((int)this + 0x718) + -1; local_34 = local_34 + 1) {
    FUN_00422e10(&local_14);
    FUN_00439330(&local_14,(float *)&local_14,local_2c[6],local_2c[7] * DAT_0056e0ec);
    FUN_004296e0(&local_14,local_2c);
    local_38 = local_38 + local_2c[7];
    if (DAT_0056cd90 <= local_38) {
      FUN_00422e10(local_20);
      fVar2 = *(float *)((int)this + 0x78) * DAT_0056e0ec;
      fVar1 = local_2c[7];
      FUN_00439330(local_20,local_20,*(float *)((int)this + 0x70),fVar1 / DAT_0056c8d0);
      FUN_004296e0(&local_14,local_20);
      *local_24 = local_14;
      local_24[1] = local_10;
      local_24[2] = local_c;
      local_24[6] = fVar1 * DAT_0056f798;
      local_24[7] = fVar2 * *(float *)((int)this + 0x131c);
      local_24[9] = local_2c[6];
      local_24[10] = local_2c[6];
      local_24[0xe] = local_24[0xe] & 0xfffffffe;
      local_24[0xe] = local_24[0xe] & 0xfffffffd;
      local_24[0xe] = local_24[0xe] | 4;
      local_24[0xb] = local_2c[7];
      local_30 = local_30 + 1;
      local_24 = local_24 + 0xf;
    }
    local_2c = local_2c + 8;
  }
  *(int *)((int)this + 0x6e0) = local_30;
  if ((local_30 != 0) && (param_1 != (uint *)0x0)) {
    FUN_00543e20(param_1,*(uint **)((int)this + 0x6d8),local_30 * 0x3c);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

