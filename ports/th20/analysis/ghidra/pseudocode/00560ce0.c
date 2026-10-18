/* Entry: 0x00560ce0; symbol: _GetLocaleNameFromLangCountry; body bytes: 153 */

/* Library Function - Single Match
    _GetLocaleNameFromLangCountry
   
   Library: Visual Studio 2019 Release */

void __cdecl _GetLocaleNameFromLangCountry(undefined4 *param_1)

{
  short *psVar1;
  short sVar2;
  uint uVar3;
  int iVar4;
  short *psVar5;
  ushort *this;
  short *psVar6;
  
  iVar4 = 2;
  psVar5 = (short *)*param_1;
  psVar1 = psVar5 + 1;
  do {
    sVar2 = *psVar5;
    psVar5 = psVar5 + 1;
  } while (sVar2 != 0);
  psVar6 = (short *)param_1[1];
  this = (ushort *)((int)psVar5 - (int)psVar1 >> 1);
  psVar1 = psVar6 + 1;
  param_1[4] = (uint)(this == (ushort *)0x3);
  do {
    sVar2 = *psVar6;
    psVar6 = psVar6 + 1;
  } while (sVar2 != 0);
  param_1[5] = (uint)((int)psVar6 - (int)psVar1 >> 1 == 3);
  if (this != (ushort *)0x3) {
    this = (ushort *)*param_1;
    iVar4 = _GetPrimaryLen(this);
  }
  param_1[3] = iVar4;
  ___acrt_EnumSystemLocalesEx_16(this,FUN_00560e06,3,0,0);
  uVar3 = param_1[2];
  if ((uVar3 >> 8 & 1) == 0 || ((uVar3 & 7) == 0 || (uVar3 >> 9 & 1) == 0)) {
    param_1[2] = 0;
  }
  return;
}

