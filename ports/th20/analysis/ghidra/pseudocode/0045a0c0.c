/* Entry: 0x0045a0c0; symbol: FUN_0045a0c0; body bytes: 102 */

undefined4 __fastcall FUN_0045a0c0(void *param_1)

{
  if (*(int *)((int)param_1 + 0x20) == 2) {
    *(int *)((int)param_1 + 0x18) = *(int *)((int)param_1 + 0x18) + -1;
    if (*(int *)((int)param_1 + 0x18) < 1) {
      *(undefined4 *)((int)param_1 + 0x20) = 0;
      return 1;
    }
    FUN_0045b9b0(param_1,-((*(int *)((int)param_1 + 0x18) * 5000) / *(int *)((int)param_1 + 0x1c)));
  }
  return 0;
}

