/* Entry: 0x0052f540; symbol: FUN_0052f540; body bytes: 70 */

undefined * __cdecl FUN_0052f540(int param_1)

{
  undefined4 local_8;
  
  if (*(int *)(DAT_005c685c + param_1 * 0x704) == param_1) {
    local_8 = FUN_0052f060((byte *)(DAT_005c685c + 4 + param_1 * 0x704));
  }
  else {
    local_8 = (undefined *)0x0;
  }
  return local_8;
}

