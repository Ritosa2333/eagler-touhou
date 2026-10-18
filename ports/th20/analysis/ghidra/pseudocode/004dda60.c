/* Entry: 0x004dda60; symbol: FUN_004dda60; body bytes: 90 */

undefined4 __fastcall FUN_004dda60(int param_1)

{
  undefined4 uVar1;
  
  if (*(int *)(param_1 + 0xdb8) == 0) {
    uVar1 = 0;
  }
  else {
    FUN_004455c0(DAT_005c0028);
    *(undefined4 *)(param_1 + 0xdb8) = 0;
    uVar1 = (**(code **)(**(int **)(param_1 + 8) + 0xe4))(*(undefined4 *)(param_1 + 8),0x1c,0);
  }
  return uVar1;
}

