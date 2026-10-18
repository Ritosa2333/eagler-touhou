/* Entry: 0x004f4ec0; symbol: FUN_004f4ec0; body bytes: 46 */

uint * __fastcall FUN_004f4ec0(uint *param_1)

{
  *param_1 = *param_1 & 0xf0000000;
  *param_1 = *param_1 & 0xfffffff;
  return param_1;
}

