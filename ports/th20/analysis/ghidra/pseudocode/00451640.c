/* Entry: 0x00451640; symbol: FUN_00451640; body bytes: 183 */

void __cdecl FUN_00451640(int *param_1,ushort *param_2,int *param_3)

{
  if ((param_1 != (int *)0x0) && ((short)*param_2 < 0)) {
    *param_1 = (uint)(*param_2 >> 10 & 0x1f) + *param_1;
    param_1[1] = (uint)(*param_2 >> 5 & 0x1f) + param_1[1];
    param_1[2] = (uint)(*param_2 & 0x1f) + param_1[2];
    *param_3 = *param_3 + 1;
  }
  return;
}

