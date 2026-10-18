/* Entry: 0x004694f0; symbol: FUN_004694f0; body bytes: 322 */

ulonglong __cdecl FUN_004694f0(int param_1,uint param_2,undefined1 *param_3)

{
  uint uVar1;
  ulonglong uVar2;
  int local_8;
  
  local_8 = 0;
  while (9999 < param_2) {
    uVar1 = param_2 % 10000;
    param_2 = param_2 / 10000;
    FUN_00543e20((uint *)(param_3 + (param_1 - local_8) + -2),
                 (uint *)(&DAT_0056f350 + (uVar1 % 100) * 2),2);
    FUN_00543e20((uint *)(param_3 + (param_1 - local_8) + -4),
                 (uint *)(&DAT_0056f350 + (uVar1 / 100) * 2),2);
    local_8 = local_8 + 4;
  }
  if (99 < param_2) {
    uVar1 = param_2 % 100;
    param_2 = param_2 / 100;
    FUN_00543e20((uint *)(param_3 + (param_1 - local_8) + -2),(uint *)(&DAT_0056f350 + uVar1 * 2),2)
    ;
    local_8 = local_8 + 2;
  }
  if (param_2 < 10) {
    uVar2 = CONCAT44(param_2 + 0x30,param_3);
    *param_3 = (char)(param_2 + 0x30);
  }
  else {
    uVar2 = FUN_00543e20((uint *)(param_3 + (param_1 - local_8) + -2),
                         (uint *)(&DAT_0056f350 + param_2 * 2),2);
  }
  return uVar2;
}

