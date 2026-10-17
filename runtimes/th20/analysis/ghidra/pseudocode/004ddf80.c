/* Entry: 0x004ddf80; symbol: FUN_004ddf80; body bytes: 90 */

undefined4 __fastcall FUN_004ddf80(int param_1)

{
  undefined4 uVar1;
  
  if (*(int *)(param_1 + 0xdbc) == 0) {
    uVar1 = 0;
  }
  else {
    FUN_004455c0(DAT_005c0028);
    *(undefined4 *)(param_1 + 0xdbc) = 0;
    uVar1 = (**(code **)(**(int **)(param_1 + 8) + 0xe4))(*(undefined4 *)(param_1 + 8),0xe,0);
  }
  return uVar1;
}

