/* Entry: 0x0052cb30; symbol: FUN_0052cb30; body bytes: 158 */

undefined4 FUN_0052cb30(int param_1,int param_2)

{
  int iVar1;
  int local_8;
  
  local_8 = 0;
  while ((local_8 < 5 &&
         (-1 < *(int *)(&DAT_005af220 + local_8 * 4 + param_2 * 0x14 + param_1 * 0x104)))) {
    iVar1 = FUN_00488700(DAT_005c6108);
    iVar1 = FUN_00486e10((void *)(iVar1 + 0xb08),
                         *(int *)(&DAT_005af220 + local_8 * 4 + param_2 * 0x14 + param_1 * 0x104));
    if (*(int *)(iVar1 + 200) != 0) {
      return 1;
    }
    local_8 = local_8 + 1;
  }
  return 0;
}

