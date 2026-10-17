/* Entry: 0x004974d0; symbol: FUN_004974d0; body bytes: 108 */

uint __thiscall FUN_004974d0(void *this,int param_1,undefined4 param_2,int param_3)

{
  undefined4 *puVar1;
  undefined4 local_18;
  undefined4 local_14;
  void *local_10;
  void *local_c;
  uint local_8;
  
  local_c = this;
  local_8 = FUN_00498e70((int)this);
  if ((local_8 == 0xffffffff) || (0x3ff < (int)local_8)) {
    local_8 = 0;
  }
  else {
    puVar1 = FUN_0049db70(&local_18,param_1,param_2,param_3);
    local_14 = *puVar1;
    local_10 = (void *)((int)local_c + 0x3c);
    puVar1 = (undefined4 *)FUN_00414580(local_10,local_8);
    *puVar1 = local_14;
    local_8 = local_8 | 0x80000000;
  }
  return local_8;
}

