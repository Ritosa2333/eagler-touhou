/* Entry: 0x004b9b80; symbol: FUN_004b9b80; body bytes: 137 */

uint * __fastcall FUN_004b9b80(uint *param_1)

{
  *param_1 = *param_1 & 0xfffffffe;
  *param_1 = *param_1 & 0xfffffffd;
  *param_1 = *param_1 & 0xfffffffb;
  *param_1 = *param_1 & 0xfffffff7;
  *param_1 = *param_1 & 0xffffffef;
  *param_1 = *param_1 & 0xffffffdf;
  *param_1 = *param_1 & 0xffffffbf;
  *param_1 = *param_1 | 0x80;
  *param_1 = *param_1 & 0xfffffeff;
  return param_1;
}

