/* Entry: 0x004d9f60; symbol: FUN_004d9f60; body bytes: 435 */

void FUN_004d9f60(float *param_1)

{
  int *piVar1;
  undefined1 local_2c [12];
  float local_20 [3];
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (DAT_005c0028 != 0) {
    FUN_004455c0(DAT_005c0028);
  }
  local_14 = param_1[9];
  local_10 = param_1[10];
  local_c = param_1[0xb];
  FUN_00429570(param_1 + 0xf,local_20,param_1);
  FUN_00429570(&local_14,local_2c,local_20);
  D3DXMatrixLookAtLH(param_1 + 0x18,local_20,local_2c,param_1 + 6);
  D3DXMatrixPerspectiveFovLH
            (param_1 + 0x28,param_1[0x15],
             (float)((double)(int)param_1[0x3a] +
                    *(double *)(&DAT_0056cdd0 + ((int)param_1[0x3a] >> 0x1f) * -8)) /
             (float)((double)(int)param_1[0x3b] +
                    *(double *)(&DAT_0056cdd0 + ((int)param_1[0x3b] >> 0x1f) * -8)),DAT_0056f280,
             DAT_00571e9c);
  piVar1 = (int *)FUN_00412730(0x5c4d40);
  (**(code **)(*piVar1 + 0xb0))(piVar1,2,param_1 + 0x18);
  piVar1 = (int *)FUN_00412730(0x5c4d40);
  (**(code **)(*piVar1 + 0xb0))(piVar1,3,param_1 + 0x28);
  FUN_004d9680(param_1 + 0xc,&local_14,param_1 + 6);
  FUN_0045cf10(param_1 + 0xc,param_1 + 0xc);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

