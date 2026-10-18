/* Entry: 0x004da1f0; symbol: FUN_004da1f0; body bytes: 846 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004da1f0(int param_1,int *param_2)

{
  float10 fVar1;
  double dVar2;
  undefined1 local_2c [12];
  undefined1 local_20 [12];
  undefined1 local_14 [12];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  dVar2 = (double)(int)((uint)param_2[3] >> 1) + _DAT_0056cdd0;
  fVar1 = FUN_00439850((void *)((uint)param_2[3] >> 1),*(float *)(param_1 + 0x54) / DAT_0056c8d0);
  FUN_00422dd0(local_2c,(float)((double)*param_2 +
                               *(double *)(&DAT_0056cdd0 + (*param_2 >> 0x1f) * -8)) +
                        (float)((double)param_2[2] +
                               *(double *)(&DAT_0056cdd0 + (param_2[2] >> 0x1f) * -8)) /
                        DAT_0056c8d0,
               (float)((double)param_2[1] + *(double *)(&DAT_0056cdd0 + (param_2[1] >> 0x1f) * -8))
               + (float)((double)param_2[3] + *(double *)(&DAT_0056cdd0 + (param_2[3] >> 0x1f) * -8)
                        ) / DAT_0056c8d0,(float)dVar2 / (float)fVar1);
  FUN_00422dd0(local_20,(float)((double)*(int *)(param_1 + 0xe0) +
                               *(double *)(&DAT_0056cdd0 + (*(int *)(param_1 + 0xe0) >> 0x1f) * -8))
                        + (float)((double)*(int *)(param_1 + 0xe8) +
                                 *(double *)
                                  (&DAT_0056cdd0 + (*(int *)(param_1 + 0xe8) >> 0x1f) * -8)) /
                          DAT_0056c8d0,
               (float)((double)*(int *)(param_1 + 0xe4) +
                      *(double *)(&DAT_0056cdd0 + (*(int *)(param_1 + 0xe4) >> 0x1f) * -8)) +
               (float)((double)*(int *)(param_1 + 0xec) +
                      *(double *)(&DAT_0056cdd0 + (*(int *)(param_1 + 0xec) >> 0x1f) * -8)) /
               DAT_0056c8d0,0);
  FUN_00422dd0(local_14,0,DAT_0056e0f4,0);
  D3DXMatrixLookAtLH(param_1 + 0x60,local_2c,local_20,local_14);
  D3DXMatrixPerspectiveFovLH
            (param_1 + 0xa0,*(undefined4 *)(param_1 + 0x54),
             (float)((double)param_2[2] + *(double *)(&DAT_0056cdd0 + (param_2[2] >> 0x1f) * -8)) /
             (float)((double)param_2[3] + *(double *)(&DAT_0056cdd0 + (param_2[3] >> 0x1f) * -8)),
             DAT_0056c8cc,DAT_00571ea0);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

