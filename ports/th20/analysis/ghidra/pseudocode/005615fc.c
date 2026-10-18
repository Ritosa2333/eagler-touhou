/* Entry: 0x005615fc; symbol: _GetLcidFromCountry; body bytes: 75 */

/* Library Function - Single Match
    _GetLcidFromCountry
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl _GetLcidFromCountry(byte *param_1)

{
  short *psVar1;
  short sVar2;
  int iVar3;
  short *psVar4;
  
  iVar3 = FUN_005579c2();
  psVar4 = *(short **)(iVar3 + 0x54);
  psVar1 = psVar4 + 1;
  do {
    sVar2 = *psVar4;
    psVar4 = psVar4 + 1;
  } while (sVar2 != 0);
  *(uint *)(iVar3 + 100) = (uint)((int)psVar4 - (int)psVar1 >> 1 == 3);
  EnumSystemLocalesW(FUN_00561559,1);
  if ((*param_1 & 4) == 0) {
    param_1[0] = 0;
    param_1[1] = 0;
    param_1[2] = 0;
    param_1[3] = 0;
  }
  return;
}

