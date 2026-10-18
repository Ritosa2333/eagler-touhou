/* Entry: 0x00478f00; symbol: FUN_00478f00; body bytes: 103 */

undefined4 * __thiscall FUN_00478f00(void *this,undefined4 *param_1)

{
  undefined4 *puVar1;
  undefined1 local_28 [12];
  undefined1 local_1c [12];
  void *local_10;
  void *local_c;
  void *local_8;
  
  local_c = (void *)((int)this + 0x5bc);
  local_8 = this;
  local_10 = FUN_00429570(local_c,local_1c,(float *)((int)this + 0x2c));
  puVar1 = (undefined4 *)FUN_00429570(local_10,local_28,(float *)((int)local_8 + 0x484));
  *param_1 = *puVar1;
  param_1[1] = puVar1[1];
  param_1[2] = puVar1[2];
  FUN_00478f70(local_8,param_1);
  return param_1;
}

