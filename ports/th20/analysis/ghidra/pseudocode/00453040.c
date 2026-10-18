/* Entry: 0x00453040; symbol: FUN_00453040; body bytes: 94 */

void __fastcall FUN_00453040(float *param_1)

{
  float10 fVar1;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422e10(&local_14);
  fVar1 = FUN_004292e0((float *)&stack0x00000004);
  FUN_00458fa0(&local_14,&local_14,param_1,(float)fVar1);
  *param_1 = local_14;
  param_1[1] = local_10;
  param_1[2] = local_c;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

