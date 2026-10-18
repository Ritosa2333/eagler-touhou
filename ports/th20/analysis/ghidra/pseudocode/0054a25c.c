/* Entry: 0x0054a25c; symbol: FUN_0054a25c; body bytes: 29 */

void __fastcall FUN_0054a25c(void *param_1)

{
  if ((*(uint *)((int)param_1 + 0x1c) >> 5 & 1) != 0) {
    *(uint *)((int)param_1 + 0x1c) = *(uint *)((int)param_1 + 0x1c) | 0x80;
  }
  FUN_00548c97(param_1);
  return;
}

