/* Entry: 0x005602f1; symbol: FUN_005602f1; body bytes: 37 */

void __cdecl FUN_005602f1(undefined4 *param_1,int param_2)

{
  undefined4 *puVar1;
  
  puVar1 = param_1 + param_2;
  for (; param_1 != puVar1; param_1 = param_1 + 1) {
    FUN_00557ba0((LPVOID)*param_1);
  }
  return;
}

