/* Entry: 0x00484700; symbol: FUN_00484700; body bytes: 20 */

void __fastcall FUN_00484700(int param_1)

{
  LOCK();
  *(int *)(param_1 + 4) = *(int *)(param_1 + 4) + 1;
  UNLOCK();
  return;
}

