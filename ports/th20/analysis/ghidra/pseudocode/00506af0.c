/* Entry: 0x00506af0; symbol: FUN_00506af0; body bytes: 166 */

void __fastcall FUN_00506af0(char **param_1)

{
  char *pcVar1;
  int iVar2;
  char **ppcVar3;
  
  param_1[0x27] = (char *)0x0;
  FUN_0044fcd0(param_1 + 6);
  iVar2 = FUN_00412710((int)param_1[0x48]);
  if (iVar2 != 0) {
    ppcVar3 = (char **)FUN_004c0e10(param_1 + 0x37);
    if (ppcVar3 != (char **)0x0) {
      FUN_004c1f30(ppcVar3);
    }
  }
  FUN_00411ce0((int)param_1);
  FUN_00506ab0(param_1[0x47],param_1);
  pcVar1 = param_1[5];
  param_1[5] = (char *)0x0;
  if (((uint)pcVar1 & 0x1000000) != 0) {
    FUN_00413b70(DAT_005b8894,param_1);
  }
  return;
}

