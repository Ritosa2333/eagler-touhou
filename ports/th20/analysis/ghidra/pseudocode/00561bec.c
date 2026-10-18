/* Entry: 0x00561bec; symbol: FUN_00561bec; body bytes: 117 */

bool __cdecl FUN_00561bec(uint param_1,int param_2)

{
  short *psVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  short *psVar5;
  bool bVar6;
  uint local_8;
  
  iVar3 = FUN_005579c2();
  iVar4 = GetLocaleInfoW(param_1 & 0x3ff | 0x400,0x20000001,(LPWSTR)&local_8,2);
  if (iVar4 == 0) {
    bVar6 = false;
  }
  else if ((param_1 == local_8) || (param_2 == 0)) {
    bVar6 = true;
  }
  else {
    psVar5 = *(short **)(iVar3 + 0x50);
    psVar1 = psVar5 + 1;
    do {
      sVar2 = *psVar5;
      psVar5 = psVar5 + 1;
    } while (sVar2 != 0);
    iVar3 = _GetPrimaryLen(*(ushort **)(iVar3 + 0x50));
    bVar6 = iVar3 != (int)psVar5 - (int)psVar1 >> 1;
  }
  return bVar6;
}

