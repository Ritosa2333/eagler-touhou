/* Entry: 0x0044c510; symbol: FUN_0044c510; body bytes: 94 */

void __thiscall FUN_0044c510(void *this,int *param_1)

{
  if (*param_1 != 0) {
    (**(code **)(*(int *)*param_1 + 8))(*param_1,this);
    *param_1 = 0;
  }
  if ((param_1[1] != 0) && (param_1[1] != 0)) {
    FUN_0041f670((LPVOID)param_1[1]);
    param_1[1] = 0;
  }
  return;
}

