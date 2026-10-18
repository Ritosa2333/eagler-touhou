/* Entry: 0x00485a60; symbol: FUN_00485a60; body bytes: 122 */

void __cdecl FUN_00485a60(void *param_1,float *param_2,float param_3)

{
  float *pfVar1;
  float10 fVar2;
  undefined1 local_28 [12];
  float local_1c;
  uint local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422e10(&local_14);
  pfVar1 = (float *)FUN_00429440(param_1,local_28,param_2);
  local_14 = *pfVar1;
  local_10 = pfVar1[1];
  local_c = pfVar1[2];
  fVar2 = FUN_00456130(&local_14);
  local_1c = (float)fVar2;
  local_18 = (uint)(local_1c <= param_3 * param_3);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

