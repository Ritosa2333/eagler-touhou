/* Entry: 0x004678c0; symbol: FUN_004678c0; body bytes: 197 */

void __cdecl FUN_004678c0(undefined4 *param_1,float *param_2,float param_3)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  float10 fVar3;
  undefined1 local_28 [8];
  undefined1 local_20 [8];
  undefined1 local_18 [8];
  void *local_10;
  float local_c;
  float local_8;
  
  fVar3 = FUN_00446b30(param_2,*param_2 * *param_2 + param_2[1] * param_2[1]);
  local_8 = (float)fVar3;
  fVar3 = FUN_00445680(local_8);
  local_c = (float)fVar3;
  if (DAT_0056e1a0 <= local_c) {
    local_10 = FUN_004673b0(param_2,local_20,local_8);
    puVar2 = (undefined4 *)FUN_004293c0(local_10,local_28,param_3);
    uVar1 = puVar2[1];
    *param_1 = *puVar2;
    param_1[1] = uVar1;
  }
  else {
    puVar2 = (undefined4 *)FUN_004293c0(param_2,local_18,param_3);
    uVar1 = puVar2[1];
    *param_1 = *puVar2;
    param_1[1] = uVar1;
  }
  return;
}

