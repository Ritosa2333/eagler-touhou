/* Entry: 0x004f5600; symbol: FUN_004f5600; body bytes: 80 */

void __fastcall FUN_004f5600(int param_1)

{
  undefined4 *puVar1;
  
  if ((*(int *)(param_1 + 4) != 0) &&
     (puVar1 = (undefined4 *)(**(code **)(**(int **)(param_1 + 4) + 8))(),
     puVar1 != (undefined4 *)0x0)) {
    (**(code **)*puVar1)(1);
  }
  return;
}

