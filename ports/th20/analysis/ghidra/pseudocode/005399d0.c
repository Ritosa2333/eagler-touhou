/* Entry: 0x005399d0; symbol: FUN_005399d0; body bytes: 64 */

void __cdecl FUN_005399d0(int param_1)

{
  DAT_005e0b40 = param_1;
  *(undefined4 *)(&DAT_005c8b38 + param_1 * 0xc) = 0x2000;
  *(undefined4 *)(&DAT_005c8b40 + param_1 * 0xc) = 0;
  *(undefined4 *)(&DAT_005c8b3c + param_1 * 0xc) = 0;
  return;
}

