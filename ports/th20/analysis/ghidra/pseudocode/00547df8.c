/* Entry: 0x00547df8; symbol: FUN_00547df8; body bytes: 60 */

void __fastcall FUN_00547df8(int *param_1)

{
  int iVar1;
  int iVar2;
  
  if ((char)param_1[5] == '\x02') {
    *(uint *)(*param_1 + 0x350) = *(uint *)(*param_1 + 0x350) & 0xfffffffd;
  }
  if ((char)param_1[7] != '\0') {
    iVar1 = param_1[6];
    iVar2 = FUN_00547ea3(param_1);
    *(int *)(iVar2 + 0x10) = iVar1;
  }
  if ((char)param_1[9] != '\0') {
    iVar1 = param_1[8];
    iVar2 = FUN_00547ea3(param_1);
    *(int *)(iVar2 + 0x14) = iVar1;
  }
  return;
}

