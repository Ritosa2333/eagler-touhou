/* Entry: 0x00464270; symbol: FUN_00464270; body bytes: 68 */

undefined1 __fastcall FUN_00464270(int param_1)

{
  char cVar1;
  
  if ((*(int *)(param_1 + 0x28) != 0) &&
     (cVar1 = (**(code **)(**(int **)(param_1 + 0x28) + 0x68))(), cVar1 != '\0')) {
    return 1;
  }
  return 0;
}

