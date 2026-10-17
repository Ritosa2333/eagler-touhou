/* Entry: 0x00451880; symbol: FUN_00451880; body bytes: 139 */

void __cdecl FUN_00451880(int *param_1,byte *param_2,int *param_3)

{
  if ((param_1 != (int *)0x0) && (param_2[3] != 0)) {
    *param_1 = (uint)param_2[2] + *param_1;
    param_1[1] = (uint)param_2[1] + param_1[1];
    param_1[2] = (uint)*param_2 + param_1[2];
    *param_3 = *param_3 + 1;
  }
  return;
}

