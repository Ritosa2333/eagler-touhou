/* Entry: 0x0052f4e0; symbol: FUN_0052f4e0; body bytes: 88 */

undefined * __cdecl FUN_0052f4e0(int param_1,int param_2,int param_3)

{
  undefined4 local_8;
  
  if (*(int *)(DAT_005c685c + param_1 * 0x704) == param_1) {
    local_8 = FUN_0052f060((byte *)(param_1 * 0x704 + DAT_005c685c + 0x104 + param_2 * 0x300 +
                                   param_3 * 0x100));
  }
  else {
    local_8 = (undefined *)0x0;
  }
  return local_8;
}

