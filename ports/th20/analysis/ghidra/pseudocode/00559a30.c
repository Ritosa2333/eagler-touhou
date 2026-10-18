/* Entry: 0x00559a30; symbol: FUN_00559a30; body bytes: 57 */

uint __cdecl FUN_00559a30(uint param_1)

{
  return ((((param_1 & 0x3f) * 4 | param_1 & 0xffffff00) << 6 | param_1 & 0x3f) << 2 |
         param_1 & 0x300) << 0xe | param_1;
}

