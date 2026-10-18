/* Entry: 0x00469450; symbol: FUN_00469450; body bytes: 148 */

void __cdecl FUN_00469450(uint param_1,uint param_2,int param_3)

{
  uint uVar1;
  uint local_c;
  
  for (local_c = 0; local_c < param_1 - 1; local_c = local_c + 2) {
    uVar1 = param_2 % 100;
    param_2 = param_2 / 100;
    FUN_00543e20((uint *)(((param_3 + param_1) - local_c) + -2),(uint *)(&DAT_0056f350 + uVar1 * 2),
                 2);
  }
  if (local_c < param_1) {
    *(char *)(param_3 + -1 + (param_1 - local_c)) = (char)((ulonglong)param_2 % 10) + '0';
  }
  return;
}

