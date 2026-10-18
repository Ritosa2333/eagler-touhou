/* Entry: 0x004545d0; symbol: FUN_004545d0; body bytes: 524 */

void __cdecl
FUN_004545d0(float *param_1,float *param_2,float param_3,float param_4,float param_5,float param_6,
            float param_7,float param_8,float param_9,float param_10)

{
  char cVar1;
  bool bVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  float10 fVar4;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  int iVar3;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  cVar1 = FUN_00456920(param_3,param_4,param_5,param_6,param_7,param_8,param_9,param_10);
  if (cVar1 != '\0') {
    bVar2 = FUN_00456170(&local_c,&local_10,param_3,param_4,param_5,param_6);
    iVar3 = CONCAT31(extraout_var,bVar2);
    bVar2 = FUN_00456170(&local_14,&local_18,param_7,param_8,param_9,param_10);
    if ((iVar3 == 0) && (CONCAT31(extraout_var_00,bVar2) == 0)) {
      *param_1 = (local_18 - local_10) / (local_c - local_14);
      *param_2 = ((local_18 - local_10) * local_c) / (local_c - local_14) + local_10;
    }
    else if ((iVar3 == 0) || (CONCAT31(extraout_var_00,bVar2) == 0)) {
      if (iVar3 == 0) {
        *param_1 = param_7;
        *param_2 = local_c * param_7 + local_10;
      }
      else {
        *param_1 = param_3;
        *param_2 = local_14 * param_3 + local_18;
      }
    }
    else {
      fVar4 = FUN_00445680(param_3 - param_7);
      if ((float)fVar4 < DAT_0056e714) {
        *param_1 = param_3;
        *param_2 = param_4;
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

