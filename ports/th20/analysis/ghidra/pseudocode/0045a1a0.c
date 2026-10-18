/* Entry: 0x0045a1a0; symbol: FUN_0045a1a0; body bytes: 145 */

undefined4 __fastcall FUN_0045a1a0(void *param_1)

{
  if (*(int *)((int)param_1 + 0x20) == 1) {
    *(int *)((int)param_1 + 0x18) = *(int *)((int)param_1 + 0x18) + -1;
    if (*(int *)((int)param_1 + 0x18) < 1) {
      *(undefined4 *)((int)param_1 + 0x20) = 0;
      (**(code **)(*(int *)**(undefined4 **)((int)param_1 + 8) + 0x48))
                (**(undefined4 **)((int)param_1 + 8));
      return 1;
    }
    FUN_0045b9b0(param_1,(*(int *)((int)param_1 + 0x18) * 5000) / *(int *)((int)param_1 + 0x1c) +
                         -5000);
  }
  return 0;
}

