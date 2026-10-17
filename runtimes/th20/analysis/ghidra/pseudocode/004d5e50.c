/* Entry: 0x004d5e50; symbol: FUN_004d5e50; body bytes: 579 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_004d5e50(void *this,undefined4 *param_1)

{
  int iVar1;
  undefined4 *puVar2;
  float fVar3;
  float local_2c;
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
  if (((*(int *)((int)this + 0x20) == 4) || (*(int *)((int)this + 0x20) == 2)) &&
     (DAT_0056cd90 < *(float *)((int)this + 0x74))) {
    FUN_00422e10(&local_14);
    if (DAT_0056cd94 < *(float *)((int)this + 0x78) || DAT_0056cd94 == *(float *)((int)this + 0x78))
    {
      local_24 = *(float *)((int)this + 0x78) -
                 (*(float *)((int)this + 0x78) + DAT_0056cd90) / DAT_0056c8d8;
    }
    else {
      local_24 = *(float *)((int)this + 0x78) * DAT_0056e0ec;
    }
    local_2c = local_24;
    fVar3 = *(float *)((int)this + 0x74) * _DAT_0057178c;
    if ((*(uint *)((int)this + 0x1c) >> 7 & 1) != 0) {
      if (DAT_0056cd94 < *(float *)((int)this + 0x78) ||
          DAT_0056cd94 == *(float *)((int)this + 0x78)) {
        local_28 = DAT_0056c8e0;
      }
      else {
        local_28 = DAT_0056c8d0;
      }
      local_2c = *(float *)((int)this + 0x78) - local_28;
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
    puVar2[0xb] = 0;
    puVar2[6] = fVar3 * DAT_0056c8cc;
    puVar2[7] = local_2c * DAT_0056c8cc;
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

