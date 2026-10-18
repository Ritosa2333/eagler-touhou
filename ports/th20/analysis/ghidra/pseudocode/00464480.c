/* Entry: 0x00464480; symbol: FUN_00464480; body bytes: 68 */

undefined1 __fastcall FUN_00464480(int param_1)

{
  char cVar1;
  
  if ((*(int *)(param_1 + 0x30) != 0) &&
     (cVar1 = (**(code **)(**(int **)(param_1 + 0x30) + 0x6c))(), cVar1 != '\0')) {
    return 1;
  }
  return 0;
}

