/* Entry: 0x004517c0; symbol: FUN_004517c0; body bytes: 187 */

void __cdecl FUN_004517c0(int *param_1,ushort *param_2,int *param_3)

{
  if ((param_1 != (int *)0x0) && (*param_2 >> 8 != 0)) {
    *param_1 = (uint)(*param_2 >> 5 & 7) + *param_1;
    param_1[1] = (uint)(*param_2 >> 2 & 7) + param_1[1];
    param_1[2] = (uint)(*param_2 & 3) + param_1[2];
    *param_3 = *param_3 + 1;
  }
  return;
}

