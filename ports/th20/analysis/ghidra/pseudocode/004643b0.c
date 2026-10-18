/* Entry: 0x004643b0; symbol: FUN_004643b0; body bytes: 68 */

undefined1 __fastcall FUN_004643b0(int param_1)

{
  char cVar1;
  
  if ((*(int *)(param_1 + 0x34) != 0) &&
     (cVar1 = (**(code **)(**(int **)(param_1 + 0x34) + 0x74))(), cVar1 != '\0')) {
    return 1;
  }
  return 0;
}

