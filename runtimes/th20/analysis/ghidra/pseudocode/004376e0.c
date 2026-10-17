/* Entry: 0x004376e0; symbol: FUN_004376e0; body bytes: 103 */

float * __thiscall FUN_004376e0(void *this,float *param_1)

{
  float *pfVar1;
  undefined1 local_28 [12];
  undefined1 local_1c [12];
  void *local_10;
  void *local_c;
  void *local_8;
  
  local_c = (void *)((int)this + 0x5bc);
  local_8 = this;
  local_10 = FUN_00429570(local_c,local_1c,(float *)((int)this + 0x2c));
  pfVar1 = (float *)FUN_00429570(local_10,local_28,(float *)((int)local_8 + 0x484));
  *param_1 = *pfVar1;
  param_1[1] = pfVar1[1];
  param_1[2] = pfVar1[2];
  FUN_004379e0(local_8,param_1);
  return param_1;
}

