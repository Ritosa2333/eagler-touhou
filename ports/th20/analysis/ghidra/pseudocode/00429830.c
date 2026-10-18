/* Entry: 0x00429830; symbol: FUN_00429830; body bytes: 113 */

float10 __fastcall FUN_00429830(int param_1)

{
  int iVar1;
  
  iVar1 = FUN_00423ee0(param_1);
  return (float10)((float)((double)iVar1 + *(double *)(&DAT_0056cdd0 + (iVar1 >> 0x1f) * -8)) /
                  ((float)((double)*(int *)(param_1 + 0x10) +
                          *(double *)(&DAT_0056cdd0 + (*(int *)(param_1 + 0x10) >> 0x1f) * -8)) -
                  DAT_0056c8cc));
}

