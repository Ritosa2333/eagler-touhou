/* Entry: 0x00560d79; symbol: _GetLocaleNameFromLanguage; body bytes: 89 */

/* Library Function - Single Match
    _GetLocaleNameFromLanguage
   
   Library: Visual Studio 2019 Release */

void __cdecl _GetLocaleNameFromLanguage(undefined4 *param_1)

{
  short *psVar1;
  short sVar2;
  int iVar3;
  short *psVar4;
  ushort *this;
  
  iVar3 = 2;
  psVar4 = (short *)*param_1;
  psVar1 = psVar4 + 1;
  do {
    sVar2 = *psVar4;
    psVar4 = psVar4 + 1;
  } while (sVar2 != 0);
  this = (ushort *)((int)psVar4 - (int)psVar1 >> 1);
  param_1[4] = (uint)(this == (ushort *)0x3);
  if (this != (ushort *)0x3) {
    this = (ushort *)*param_1;
    iVar3 = _GetPrimaryLen(this);
  }
  param_1[3] = iVar3;
  ___acrt_EnumSystemLocalesEx_16(this,FUN_005610e6,3,0,0);
  if ((*(byte *)(param_1 + 2) & 4) == 0) {
    param_1[2] = 0;
  }
  return;
}

