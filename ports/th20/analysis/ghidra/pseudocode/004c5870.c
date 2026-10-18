/* Entry: 0x004c5870; symbol: FUN_004c5870; body bytes: 90 */

undefined4 __fastcall FUN_004c5870(int param_1)

{
  if (*(int *)(param_1 + 0xfc) == 1) {
    FUN_004c7770();
  }
  else if (*(int *)(param_1 + 0xfc) == 2) {
    FUN_004c6b30();
  }
  FUN_00488b00(DAT_005c0698,0);
  FUN_00470ad0(DAT_005c0698,0xffffffff);
  return 1;
}

