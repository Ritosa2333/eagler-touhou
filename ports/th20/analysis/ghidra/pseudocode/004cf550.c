/* Entry: 0x004cf550; symbol: FUN_004cf550; body bytes: 103 */

int __cdecl FUN_004cf550(int param_1,int param_2)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x5c8);
  if (-1 < *(int *)(&DAT_005c06ac + *(int *)(iVar1 + 0x6e4) * 0x158)) {
    param_2 = *(int *)(&DAT_005c06ac +
                      param_2 * 4 + *(int *)(iVar1 + 0x6e8) * 0x14 + *(int *)(iVar1 + 0x6e4) * 0x158
                      );
  }
  return param_2;
}

