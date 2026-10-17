/* Entry: 0x004ddac0; symbol: FUN_004ddac0; body bytes: 90 */

undefined4 __fastcall FUN_004ddac0(int param_1)

{
  undefined4 uVar1;
  
  if (*(int *)(param_1 + 0xdb8) == 1) {
    uVar1 = 0;
  }
  else {
    FUN_004455c0(DAT_005c0028);
    *(undefined4 *)(param_1 + 0xdb8) = 1;
    uVar1 = (**(code **)(**(int **)(param_1 + 8) + 0xe4))(*(undefined4 *)(param_1 + 8),0x1c,1);
  }
  return uVar1;
}

