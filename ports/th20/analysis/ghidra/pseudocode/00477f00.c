/* Entry: 0x00477f00; symbol: FUN_00477f00; body bytes: 56 */

undefined4 __fastcall FUN_00477f00(int param_1)

{
  undefined4 local_c;
  
  if (*(int *)(param_1 + 0x14) == 0) {
    local_c = 0;
  }
  else {
    local_c = (**(code **)(**(int **)(param_1 + 0x14) + 0x10))();
  }
  return local_c;
}

