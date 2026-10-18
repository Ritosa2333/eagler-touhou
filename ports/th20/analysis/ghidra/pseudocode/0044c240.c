/* Entry: 0x0044c240; symbol: FUN_0044c240; body bytes: 171 */

void __fastcall FUN_0044c240(int param_1)

{
  if (*(int *)(param_1 + 0x564) != 0) {
    FUN_0041f670(*(LPVOID *)(param_1 + 0x564));
  }
  *(undefined4 *)(param_1 + 0x564) = 0;
  *(undefined4 *)(param_1 + 0x56c) = 0;
  FUN_0041f7c0(*(undefined4 **)(param_1 + 0x568));
  *(undefined4 *)(param_1 + 0x568) = 0;
  FUN_004117a0((void *)(param_1 + 0x4c0),0);
  *(undefined4 *)(param_1 + 0x28) = 0xffffffff;
  if (*(int *)(param_1 + 0x550) == 0) {
    return;
  }
  do {
    FUN_004117a0((void *)(param_1 + 0x4c0),0);
  } while( true );
}

