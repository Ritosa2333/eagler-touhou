/* Entry: 0x00415640; symbol: FUN_00415640; body bytes: 82 */

undefined * FUN_00415640(int param_1)

{
  undefined *puVar1;
  int local_8;
  
  for (local_8 = 0;
      (*(int *)(&DAT_005ae078 + local_8 * 0x18) != -1 &&
      (*(int *)(&DAT_005ae078 + local_8 * 0x18) != param_1)); local_8 = local_8 + 1) {
  }
  if (param_1 == -1) {
    puVar1 = (undefined *)0x0;
  }
  else {
    puVar1 = &DAT_005ae078 + local_8 * 0x18;
  }
  return puVar1;
}

