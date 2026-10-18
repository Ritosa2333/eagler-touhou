/* Entry: 0x00458b60; symbol: FUN_00458b60; body bytes: 804 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
FUN_00458b60(float param_1,float param_2,float param_3,float param_4,float param_5,float *param_6)

{
  float10 fVar1;
  float local_28;
  float fStack_24;
  float fStack_20;
  float fStack_1c;
  float fStack_18;
  float fStack_14;
  float fStack_10;
  float fStack_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0040bc20(&local_28,8,4,FUN_004398a0);
  local_28 = *param_6 - param_1;
  fStack_24 = param_6[1] - param_2;
  fStack_20 = param_6[2] - param_1;
  fStack_1c = param_6[3] - param_2;
  fStack_18 = param_6[4] - param_1;
  fStack_14 = param_6[5] - param_2;
  fStack_10 = param_6[6] - param_1;
  fStack_c = param_6[7] - param_2;
  if (param_5 != _DAT_0056e0e8) {
    FUN_00459030(&local_28,&local_28,&local_28,(float)((uint)param_5 ^ _DAT_0056d7d0),4);
  }
  fVar1 = FUN_00445680(local_28);
  if ((((param_3 / DAT_0056c8d0 < (float)fVar1) ||
       (fVar1 = FUN_00445680(fStack_24), param_4 / DAT_0056c8d0 < (float)fVar1)) &&
      ((fVar1 = FUN_00445680(fStack_20), param_3 / DAT_0056c8d0 < (float)fVar1 ||
       (fVar1 = FUN_00445680(fStack_1c), param_4 / DAT_0056c8d0 < (float)fVar1)))) &&
     (((fVar1 = FUN_00445680(fStack_18), param_3 / DAT_0056c8d0 < (float)fVar1 ||
       (fVar1 = FUN_00445680(fStack_14), param_4 / DAT_0056c8d0 < (float)fVar1)) &&
      (fVar1 = FUN_00445680(fStack_10), (float)fVar1 <= param_3 / DAT_0056c8d0)))) {
    FUN_00445680(fStack_c);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

