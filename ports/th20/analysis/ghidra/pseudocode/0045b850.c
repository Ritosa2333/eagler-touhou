/* Entry: 0x0045b850; symbol: FUN_0045b850; body bytes: 349 */

/* WARNING: Removing unreachable block (ram,0x0045b8eb) */

void __thiscall FUN_0045b850(void *this,short param_1,int param_2,ushort param_3)

{
  int iVar1;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_34;
  int *local_20;
  undefined2 local_1c;
  short local_1a;
  int local_18;
  int local_14;
  ushort local_10;
  ushort local_e;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
                    /* WARNING: Load size is inaccurate */
  if (*this != 0) {
    _memset(&local_44,0,0x24);
    local_44 = 0x24;
    local_40 = 1;
    local_3c = 0;
    local_34 = 0;
    local_20 = (int *)0x0;
                    /* WARNING: Load size is inaccurate */
                    /* WARNING: Load size is inaccurate */
    iVar1 = (**(code **)(**this + 0xc))(*this,&local_44,&local_20,0);
    if ((-1 < iVar1) && (local_20 != (int *)0x0)) {
      _memset(&local_1c,0,0x12);
      local_1c = 1;
      local_1a = param_1;
      local_18 = param_2;
      local_e = param_3;
      local_10 = (short)((int)(uint)param_3 >> 3) * param_1;
      local_14 = (uint)local_10 * param_2;
      iVar1 = (**(code **)(*local_20 + 0x38))(local_20,&local_1c);
      if ((-1 < iVar1) && (local_20 != (int *)0x0)) {
        (**(code **)(*local_20 + 8))(local_20);
        local_20 = (int *)0x0;
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

