/* Entry: 0x004e6450; symbol: FUN_004e6450; body bytes: 73 */

void __fastcall FUN_004e6450(int *param_1)

{
  if ((*param_1 != 0) && (param_1[5] != 0)) {
    (**(code **)(*(int *)*param_1 + 0x30))(*param_1,0,0,*(undefined4 *)(param_1[2] + 0xc));
  }
  return;
}

