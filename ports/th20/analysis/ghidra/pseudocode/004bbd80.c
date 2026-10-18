/* Entry: 0x004bbd80; symbol: FUN_004bbd80; body bytes: 91 */

void __fastcall FUN_004bbd80(int param_1)

{
  undefined4 local_8;
  
  *(undefined4 *)(param_1 + 0x210) = 0;
  *(undefined4 *)(param_1 + 0x208) = 0;
  for (local_8 = 0; local_8 < 2; local_8 = local_8 + 1) {
    FUN_004bbde0((void *)(local_8 * 0xf0 + param_1),local_8);
  }
  return;
}

