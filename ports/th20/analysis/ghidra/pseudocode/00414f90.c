/* Entry: 0x00414f90; symbol: FUN_00414f90; body bytes: 70 */

int * __thiscall FUN_00414f90(void *this,int *param_1)

{
  FUN_0040ec90();
  *(int *)param_1[1] = *param_1;
  *(int *)(*param_1 + 4) = param_1[1];
  *(int *)((int)this + 4) = *(int *)((int)this + 4) + -1;
  return param_1;
}

