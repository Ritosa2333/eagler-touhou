/* Entry: 0x004d95b0; symbol: FUN_004d95b0; body bytes: 204 */

void __fastcall FUN_004d95b0(int param_1)

{
  if (*(int *)(param_1 + 0xdb4) == 1) {
    FUN_0044ee90(DAT_005c5b28,1);
    FUN_0044ee90(DAT_005c5b2c,1);
    FUN_0044ee90(DAT_005c5b30,1);
    FUN_004117a0(&DAT_005c5b28,0);
    FUN_004117a0(&DAT_005c5b2c,0);
    FUN_004117a0(&DAT_005c5b30,0);
    *(undefined4 *)(param_1 + 0xdb4) = 0;
  }
  if (DAT_005c4d3c != 0) {
    DAT_005c4d3c = 0;
  }
  return;
}

