/* Entry: 0x004382b0; symbol: FUN_004382b0; body bytes: 194 */

void __thiscall FUN_004382b0(void *this,void *param_1,int param_2,int param_3)

{
  FUN_00438b70(this,param_1,param_2);
  if (param_3 == 0) {
    *(undefined4 *)((int)param_1 + 0x55c) = 0;
    *(undefined4 *)((int)param_1 + 0x558) = 0;
  }
  else {
    *(uint *)((int)param_1 + 0x49c) =
         *(uint *)((int)param_1 + 0x49c) & 0xfeffffff |
         (*(uint *)(param_3 + 0x49c) >> 0x18 & 1) << 0x18;
    FUN_0040e5e0();
    *(int *)((int)param_1 + 0x55c) = param_3;
    if (*(int *)(param_3 + 0x558) != 0) {
      param_3 = *(int *)(param_3 + 0x558);
    }
    *(undefined4 *)((int)param_1 + 0x4e8) = *(undefined4 *)(param_3 + 0x4e8);
    *(int *)((int)param_1 + 0x558) = param_3;
  }
  FUN_0042b5d0(param_1);
  return;
}

