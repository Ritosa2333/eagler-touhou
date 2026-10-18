/* Entry: 0x004d60a0; symbol: FUN_004d60a0; body bytes: 573 */

void __thiscall FUN_004d60a0(void *this,undefined4 *param_1)

{
  int iVar1;
  undefined4 *puVar2;
  float local_28;
  float local_24;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  puVar2 = *(undefined4 **)((int)this + 0x6d8);
  *(uint *)((int)this + 0x1c) = *(uint *)((int)this + 0x1c) | 0x40;
  puVar2[0xe] = puVar2[0xe] & 0xfffffff7;
  if ((((*(int *)((int)this + 0x20) == 4) || (*(int *)((int)this + 0x20) == 2)) &&
      (DAT_0056cd90 < *(float *)((int)this + 0x74))) &&
     (DAT_0056c8d8 < *(float *)((int)this + 0x78))) {
    FUN_00422e10(&local_14);
    if (DAT_0056cd94 < *(float *)((int)this + 0x78) || DAT_0056cd94 == *(float *)((int)this + 0x78))
    {
      local_24 = *(float *)((int)this + 0x78) -
                 (*(float *)((int)this + 0x78) + DAT_0056cd90) / DAT_0056c8d0;
    }
    else {
      local_24 = *(float *)((int)this + 0x78) * DAT_0056e0ec;
    }
    if ((*(uint *)((int)this + 0x72c) >> 1 & 1) == 0) {
      local_28 = (*(float *)((int)this + 0x74) * DAT_0056c8e0) / DAT_0056edd4;
    }
    else {
      local_28 = *(float *)((int)this + 0x74);
    }
    FUN_00439330(&local_14,(float *)&local_14,*(float *)((int)this + 0x70),
                 *(float *)((int)this + 0x74) / DAT_0056c8d0);
    FUN_004296e0(&local_14,(float *)((int)this + 0x58));
    *puVar2 = local_14;
    puVar2[1] = local_10;
    puVar2[2] = local_c;
    puVar2[9] = *(undefined4 *)((int)this + 0x70);
    puVar2[10] = *(undefined4 *)((int)this + 0x70);
    puVar2[0xe] = puVar2[0xe] & 0xfffffffe;
    puVar2[0xe] = puVar2[0xe] & 0xfffffffd;
    puVar2[0xe] = puVar2[0xe] | 4;
    puVar2[0xe] = puVar2[0xe] | 8;
    puVar2[0xb] = *(undefined4 *)((int)this + 0x7c);
    puVar2[6] = local_28 * DAT_0056c8cc;
    puVar2[7] = local_24 * DAT_0056c8cc;
    if (param_1 != (undefined4 *)0x0) {
      for (iVar1 = 0xf; iVar1 != 0; iVar1 = iVar1 + -1) {
        *param_1 = *puVar2;
        puVar2 = puVar2 + 1;
        param_1 = param_1 + 1;
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

