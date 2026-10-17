/* Entry: 0x0041c1a0; symbol: FUN_0041c1a0; body bytes: 314 */

bool __cdecl FUN_0041c1a0(undefined4 param_1)

{
  code *pcVar1;
  uint uVar2;
  int *piVar3;
  int iVar4;
  undefined4 uVar5;
  
  DAT_005c5888 = DAT_005c5888 & 0xfffffffe;
  uVar2 = FUN_0041d0a0(0x5c4d40);
  if (uVar2 == 0) {
    piVar3 = (int *)FUN_0040ff90(0x5c4d40);
    pcVar1 = *(code **)(*piVar3 + 0x40);
    iVar4 = FUN_0041ca70(0x5c4d40);
    uVar5 = FUN_0040c300(&DAT_005b6758);
    iVar4 = (*pcVar1)(piVar3,0,1,uVar5,0x40,param_1,iVar4);
    if (iVar4 == 0) {
      DAT_005c5888 = DAT_005c5888 | 1;
      return false;
    }
    piVar3 = (int *)FUN_0040ff90(0x5c4d40);
    pcVar1 = *(code **)(*piVar3 + 0x40);
    iVar4 = FUN_0041ca70(0x5c4d40);
    uVar5 = FUN_0040c300(&DAT_005b6758);
    iVar4 = (*pcVar1)(piVar3,0,1,uVar5,0x20,param_1,iVar4);
    if (iVar4 == 0) {
      return false;
    }
  }
  piVar3 = (int *)FUN_0040ff90(0x5c4d40);
  pcVar1 = *(code **)(*piVar3 + 0x40);
  iVar4 = FUN_0041ca70(0x5c4d40);
  uVar5 = FUN_0040c300(&DAT_005b6758);
  iVar4 = (*pcVar1)(piVar3,0,2,uVar5,0x20,param_1,iVar4);
  return iVar4 != 0;
}

