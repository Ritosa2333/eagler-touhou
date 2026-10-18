/* Entry: 0x0054f94e; symbol: FUN_0054f94e; body bytes: 33 */

void __fastcall FUN_0054f94e(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x28);
  if (iVar1 == 2) {
    *(undefined1 *)(param_1 + 0x2c) = 0;
    return;
  }
  if (((iVar1 == 3) || (iVar1 == 4)) || (iVar1 == 8)) {
    *(undefined1 *)(param_1 + 0x2c) = 1;
  }
  return;
}

