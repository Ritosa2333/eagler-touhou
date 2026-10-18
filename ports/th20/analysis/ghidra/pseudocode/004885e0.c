/* Entry: 0x004885e0; symbol: FUN_004885e0; body bytes: 148 */

bool __fastcall FUN_004885e0(int param_1)

{
  return *(int *)(param_1 + 0xa8) / 100000 + -0x16 !=
         ((*(int *)(param_1 + 0xa8) / 100) % 1000 + 0x3a6) % 1000 +
         (*(int *)(param_1 + 0xa8) % 100 + 0x43) % 100;
}

