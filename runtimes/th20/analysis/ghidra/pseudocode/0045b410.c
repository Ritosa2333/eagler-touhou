/* Entry: 0x0045b410; symbol: FUN_0045b410; body bytes: 120 */

uint __fastcall FUN_0045b410(int param_1)

{
  uint uVar1;
  undefined4 local_10;
  undefined4 local_8;
  
  if (*(int *)(param_1 + 8) == 0) {
    local_10 = 0x800401f0;
  }
  else {
    local_10 = 0;
    for (local_8 = 0; local_8 < *(uint *)(param_1 + 0x14); local_8 = local_8 + 1) {
      uVar1 = (**(code **)(**(int **)(*(int *)(param_1 + 8) + local_8 * 4) + 0x34))
                        (*(undefined4 *)(*(int *)(param_1 + 8) + local_8 * 4),0);
      local_10 = uVar1 | local_10;
    }
  }
  return local_10;
}

