/* Entry: 0x004dc510; symbol: FUN_004dc510; body bytes: 218 */

int __cdecl FUN_004dc510(int param_1)

{
  char cVar1;
  int iVar2;
  
  if (((*(uint *)(param_1 + 0xb48) >> 5 & 3) == 1) &&
     (cVar1 = FUN_004dc180(param_1 + 0xd90), cVar1 == '\0')) {
    FUN_004be250(&DAT_005c4d40,3);
  }
  FUN_004277f0(&DAT_005ba830);
  FUN_004d9710(0x5ba830);
  FUN_0041fe80(DAT_005b8898);
  iVar2 = FUN_0044dfb0(DAT_005c0028);
  if (iVar2 == 0) {
    iVar2 = FUN_004dab80(0x5b6758);
    if (iVar2 != 0) {
      FUN_004d9e70(&DAT_005b6758,-1);
    }
    FUN_004dab80(0x5b6758);
    if (*(int *)(param_1 + 0xdb0) == 0) {
      iVar2 = FUN_004da540(param_1);
      if (iVar2 == 1) {
        iVar2 = 1;
      }
    }
    else if (*(int *)(param_1 + 0xdb0) == 2) {
      iVar2 = 4;
    }
    else {
      iVar2 = 1;
    }
  }
  else {
    iVar2 = 4;
  }
  return iVar2;
}

