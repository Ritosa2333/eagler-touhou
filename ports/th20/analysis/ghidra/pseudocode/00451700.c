/* Entry: 0x00451700; symbol: FUN_00451700; body bytes: 183 */

void __cdecl FUN_00451700(int *param_1,ushort *param_2,int *param_3)

{
  if ((param_1 != (int *)0x0) && (*param_2 >> 0xc != 0)) {
    *param_1 = (uint)(*param_2 >> 8 & 0xf) + *param_1;
    param_1[1] = (uint)(*param_2 >> 4 & 0xf) + param_1[1];
    param_1[2] = (uint)(*param_2 & 0xf) + param_1[2];
    *param_3 = *param_3 + 1;
  }
  return;
}

