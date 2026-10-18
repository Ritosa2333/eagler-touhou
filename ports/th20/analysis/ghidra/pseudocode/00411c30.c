/* Entry: 0x00411c30; symbol: FUN_00411c30; body bytes: 121 */

int * __fastcall FUN_00411c30(int *param_1)

{
  int iVar1;
  
  if (*param_1 != 0) {
    FUN_00412da0((void *)*param_1,0);
  }
  *param_1 = param_1[1];
  param_1[1] = *param_1;
  if (param_1[1] != 0) {
    iVar1 = FUN_0040ff90(*param_1);
    param_1[1] = iVar1;
    if (param_1[1] != 0) {
      FUN_00412da0((void *)param_1[1],param_1);
    }
  }
  return param_1;
}

