/* Entry: 0x004e1510; symbol: FUN_004e1510; body bytes: 86 */

void __fastcall FUN_004e1510(void *param_1)

{
  int iVar1;
  
  iVar1 = FUN_004e1250(param_1,1);
  if (iVar1 != 0) {
    FUN_00426d70(&DAT_005ba830,0x11,0);
    if (DAT_005c06a4 != (void *)0x0) {
      FUN_004b90e0(DAT_005c06a4,4,0);
    }
  }
  *(int *)((int)param_1 + 0xc4) = *(int *)((int)param_1 + 0xc4) + 1;
  return;
}

