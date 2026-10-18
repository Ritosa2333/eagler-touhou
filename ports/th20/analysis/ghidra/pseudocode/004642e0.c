/* Entry: 0x004642e0; symbol: FUN_004642e0; body bytes: 68 */

undefined1 __fastcall FUN_004642e0(int param_1)

{
  char cVar1;
  
  if ((*(int *)(param_1 + 0x2c) != 0) &&
     (cVar1 = (**(code **)(**(int **)(param_1 + 0x2c) + 0x70))(), cVar1 != '\0')) {
    return 1;
  }
  return 0;
}

