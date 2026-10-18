/* Entry: 0x004532c0; symbol: FUN_004532c0; body bytes: 229 */

void __cdecl FUN_004532c0(undefined4 *param_1,float *param_2,float param_3)

{
  undefined4 *puVar1;
  float10 fVar2;
  undefined1 local_34 [12];
  undefined1 local_28 [12];
  undefined1 local_1c [12];
  void *local_10;
  float local_c;
  float local_8;
  
  fVar2 = FUN_00446b30(param_2,*param_2 * *param_2 + param_2[1] * param_2[1] +
                               param_2[2] * param_2[2]);
  local_8 = (float)fVar2;
  fVar2 = FUN_00445680(local_8);
  local_c = (float)fVar2;
  if (DAT_0056e1a0 <= local_c) {
    local_10 = FUN_00452ff0(param_2,local_28,local_8);
    puVar1 = (undefined4 *)FUN_004292f0(local_10,local_34,param_3);
    *param_1 = *puVar1;
    param_1[1] = puVar1[1];
    param_1[2] = puVar1[2];
  }
  else {
    puVar1 = (undefined4 *)FUN_004292f0(param_2,local_1c,param_3);
    *param_1 = *puVar1;
    param_1[1] = puVar1[1];
    param_1[2] = puVar1[2];
  }
  return;
}

