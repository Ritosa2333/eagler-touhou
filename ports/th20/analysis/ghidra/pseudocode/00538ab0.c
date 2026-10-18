/* Entry: 0x00538ab0; symbol: FUN_00538ab0; body bytes: 53 */

void __fastcall FUN_00538ab0(int param_1)

{
  if (*(int *)(param_1 + 4) != -1) {
    CloseHandle(*(HANDLE *)(param_1 + 4));
    *(undefined4 *)(param_1 + 4) = 0xffffffff;
    *(undefined4 *)(param_1 + 8) = 0;
  }
  return;
}

