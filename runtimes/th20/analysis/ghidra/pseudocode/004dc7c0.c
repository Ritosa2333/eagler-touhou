/* Entry: 0x004dc7c0; symbol: FUN_004dc7c0; body bytes: 240 */

undefined4 FUN_004dc7c0(void)

{
  int local_8;
  
  FUN_004455c0(DAT_005c0028);
  for (local_8 = 0; local_8 < 3; local_8 = local_8 + 1) {
    (&DAT_005c5528)[local_8 * 2] = 0;
    (&DAT_005c552c)[local_8 * 2] = 0;
    *(undefined4 *)(&DAT_005c53bc + local_8 * 8) = 0;
    *(undefined4 *)(&DAT_005c53c0 + local_8 * 8) = 0;
    *(undefined4 *)(&DAT_005c50e4 + local_8 * 8) = 0;
    *(undefined4 *)(&DAT_005c50e8 + local_8 * 8) = 0;
    *(undefined4 *)(&DAT_005c5250 + local_8 * 8) = 0;
    *(undefined4 *)(&DAT_005c5254 + local_8 * 8) = 0;
  }
  return 1;
}

