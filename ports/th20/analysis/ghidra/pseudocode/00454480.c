/* Entry: 0x00454480; symbol: FUN_00454480; body bytes: 322 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
FUN_00454480(float *param_1,float *param_2,float *param_3,float param_4,float *param_5,float param_6
            )

{
  void *this;
  void *extraout_ECX;
  void *extraout_ECX_00;
  void *pvVar1;
  float10 fVar2;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_14 = *param_5;
  local_10 = param_5[1];
  local_c = param_5[2];
  FUN_00429740(&local_14,param_3);
  FUN_00458fa0(this,&local_14,&local_14,(float)((uint)param_4 ^ _DAT_0056d7d0));
  fVar2 = FUN_00445680(local_10);
  if ((param_6 < (float)fVar2) || (local_14 < (float)((uint)param_6 ^ _DAT_0056d7d0))) {
LAB_00454530:
    *param_1 = 0.0;
    *param_2 = 0.0;
  }
  else {
    pvVar1 = extraout_ECX;
    if (local_14 < 0.0) {
      fVar2 = FUN_00456130(&local_14);
      pvVar1 = extraout_ECX_00;
      if (param_6 * param_6 < (float)fVar2) goto LAB_00454530;
    }
    fVar2 = FUN_00446b30(pvVar1,DAT_0056c8cc - (local_10 / param_6) * (local_10 / param_6));
    *param_1 = local_14 - param_6 * (float)fVar2;
    *param_2 = local_14 + param_6 * (float)fVar2;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

