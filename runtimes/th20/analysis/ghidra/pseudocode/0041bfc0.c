/* Entry: 0x0041bfc0; symbol: FUN_0041bfc0; body bytes: 88 */

void __fastcall FUN_0041bfc0(void *param_1)

{
  int local_8;
  
  for (local_8 = 0; local_8 < 4; local_8 = local_8 + 1) {
    if (-1 < *(int *)((int)param_1 + local_8 * 0x28 + 0x18)) {
      FUN_0044bb10(param_1,(int *)((int)param_1 + local_8 * 0x28 + 0x18));
      *(undefined4 *)((int)param_1 + local_8 * 0x28 + 0x18) = 0xffffffff;
    }
  }
  return;
}

