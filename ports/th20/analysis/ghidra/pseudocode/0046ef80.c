/* Entry: 0x0046ef80; symbol: FUN_0046ef80; body bytes: 376 */

void __cdecl FUN_0046ef80(int param_1,uint param_2,char *param_3)

{
  uint uVar1;
  int local_8;
  
  local_8 = 0;
  while (9999 < param_2) {
    uVar1 = param_2 % 10000;
    param_2 = param_2 / 10000;
    FUN_00543e20((uint *)(param_3 + (param_1 - local_8) + -1),
                 (uint *)(&DAT_0056f350 + (uVar1 % 100) * 2),2);
    FUN_00543e20((uint *)(param_3 + (param_1 - local_8) + -3),
                 (uint *)(&DAT_0056f350 + (uVar1 / 100) * 2),2);
    local_8 = local_8 + 4;
  }
  if (99 < param_2) {
    uVar1 = param_2 % 100;
    param_2 = param_2 / 100;
    FUN_00543e20((uint *)(param_3 + (param_1 - local_8) + -1),(uint *)(&DAT_0056f350 + uVar1 * 2),2)
    ;
  }
  if (param_2 < 10) {
    param_3[1] = '.';
    *param_3 = (char)param_2 + '0';
  }
  else {
    param_3[2] = (&DAT_0056f351)[param_2 * 2];
    param_3[1] = '.';
    *param_3 = (&DAT_0056f350)[param_2 * 2];
  }
  return;
}

