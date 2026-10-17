/* Entry: 0x00428380; symbol: FUN_00428380; body bytes: 46 */

void __fastcall FUN_00428380(int *param_1)

{
  if (*param_1 != 0) {
    (**(code **)(*(int *)*param_1 + 8))(*param_1);
    *param_1 = 0;
  }
  return;
}

