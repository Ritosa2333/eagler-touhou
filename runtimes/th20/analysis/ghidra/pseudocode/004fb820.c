/* Entry: 0x004fb820; symbol: FUN_004fb820; body bytes: 147 */

uint __thiscall FUN_004fb820(void *this,int param_1,int param_2,float *param_3,undefined4 param_4)

{
  undefined4 *puVar1;
  undefined4 local_20;
  undefined4 local_1c;
  void *local_18;
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
    local_10 = (void *)((int)local_c + 0x10);
    puVar1 = (undefined4 *)FUN_00414580(local_10,param_1);
    local_14 = *puVar1;
    puVar1 = FUN_004790e0(&local_20,(uint *)0x0,param_2,param_3,param_4,-1,(undefined4 *)0x0);
    local_1c = *puVar1;
    local_18 = (void *)((int)local_c + 0x3c);
    puVar1 = (undefined4 *)FUN_00414580(local_18,local_8);
    *puVar1 = local_1c;
    local_8 = local_8 | 0x80000000;
  }
  return local_8;
}

