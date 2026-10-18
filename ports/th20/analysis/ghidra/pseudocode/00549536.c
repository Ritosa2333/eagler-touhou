/* Entry: 0x00549536; symbol: FUN_00549536; body bytes: 61 */

undefined1 FUN_00549536(char param_1,byte param_2)

{
  byte bVar1;
  
  if ((byte)(param_1 - 0x20U) < 0x5b) {
    bVar1 = (&DAT_00597c61)[((int)param_1 - 0x20U & 0x7f) * 2];
  }
  else {
    bVar1 = 0;
  }
  return (&DAT_00597c60)[((uint)bVar1 * 9 + (uint)param_2 & 0x7f) * 2];
}

