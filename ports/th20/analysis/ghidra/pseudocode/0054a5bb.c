/* Entry: 0x0054a5bb; symbol: FUN_0054a5bb; body bytes: 66 */

undefined8 __cdecl
FUN_0054a5bb(byte param_1,int param_2,undefined4 param_3,uint param_4,undefined4 *param_5)

{
  param_5[1] = (param_2 + 0x3ffU & 0x7ff | (uint)param_1 << 0xb) << 0x14 ^ param_4 & 0xfffff;
  *param_5 = param_3;
  return 0;
}

