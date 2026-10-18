/* Entry: 0x0045a4a0; symbol: FUN_0045a4a0; body bytes: 220 */

void __fastcall FUN_0045a4a0(int param_1)

{
  uint local_14;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (*(int *)(param_1 + 8) != 0) {
    for (local_14 = 0; local_14 < *(uint *)(param_1 + 0x14); local_14 = local_14 + 1) {
      if (*(int *)(*(int *)(param_1 + 8) + local_14 * 4) != 0) {
        local_c = 0;
        (**(code **)(**(int **)(*(int *)(param_1 + 8) + local_14 * 4) + 0x24))
                  (*(undefined4 *)(*(int *)(param_1 + 8) + local_14 * 4),&local_c);
        if ((local_c & 1) == 0) break;
      }
    }
    if (local_14 == *(uint *)(param_1 + 0x14)) {
      FUN_0055020d();
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

