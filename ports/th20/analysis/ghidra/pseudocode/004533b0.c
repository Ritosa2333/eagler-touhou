/* Entry: 0x004533b0; symbol: FUN_004533b0; body bytes: 324 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_004533b0(undefined4 *param_1)

{
  float *pfVar1;
  float10 fVar2;
  
  if ((param_1[3] & 1) == 0) {
    FUN_00423f50(param_1);
    FUN_00423fe0(param_1,0);
    param_1[3] = param_1[3] | 1;
  }
  if (((uint)param_1[3] >> 1 & 3) != 0) {
    param_1[3] = param_1[3] & 0xfffffff9;
  }
  pfVar1 = (float *)(&PTR_DAT_005aefe0)[(uint)param_1[3] >> 1 & 3];
  *param_1 = param_1[1];
  if (pfVar1 == (float *)0x0) {
    param_1[1] = param_1[1] + 1;
    param_1[2] = (float)param_1[2] + DAT_0056c8cc;
  }
  else {
    fVar2 = FUN_004292e0(pfVar1);
    if (((float)fVar2 <= _DAT_0056e72c) ||
       (fVar2 = FUN_004292e0(pfVar1), DAT_0056e730 <= (float)fVar2)) {
      fVar2 = FUN_004292e0(pfVar1);
      param_1[2] = (float)param_1[2] + (float)fVar2;
      param_1[1] = (int)(float)param_1[2];
    }
    else {
      param_1[1] = param_1[1] + 1;
      param_1[2] = (float)param_1[2] + DAT_0056c8cc;
    }
  }
  return param_1[1];
}

