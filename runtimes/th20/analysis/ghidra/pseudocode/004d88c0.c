/* Entry: 0x004d88c0; symbol: FUN_004d88c0; body bytes: 204 */

uint * __fastcall FUN_004d88c0(uint *param_1)

{
  *param_1 = *param_1 & 0xfffffffe;
  *param_1 = *param_1 & 0xfffffffd;
  *param_1 = *param_1 & 0xfffffffb;
  *param_1 = *param_1 & 0xfffffff7;
  *param_1 = *param_1 & 0xffffffef;
  *param_1 = *param_1 & 0xffffff9f;
  *param_1 = *param_1 | 0x80;
  *param_1 = *param_1 & 0xfffffeff;
  *param_1 = *param_1 & 0xfffffdff;
  *param_1 = *param_1 & 0xfffffbff;
  *param_1 = *param_1 | 0x800;
  *param_1 = *param_1 & 0xffffefff;
  *param_1 = *param_1 & 0xffffdfff;
  return param_1;
}

