/* Entry: 0x00560c59; symbol: _GetLocaleNameFromDefault; body bytes: 134 */

/* Library Function - Single Match
    _GetLocaleNameFromDefault
   
   Library: Visual Studio 2019 Release */

void __thiscall _GetLocaleNameFromDefault(void *this,int param_1)

{
  short sVar1;
  int iVar2;
  short *psVar3;
  short local_b4;
  undefined1 local_b2 [170];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *(uint *)(param_1 + 8) = *(uint *)(param_1 + 8) | 0x104;
  iVar2 = ___acrt_GetUserDefaultLocaleName_8(this,&local_b4,0x55);
  if (1 < iVar2) {
    psVar3 = &local_b4;
    do {
      sVar1 = *psVar3;
      psVar3 = psVar3 + 1;
    } while (sVar1 != 0);
    iVar2 = FUN_0055f860((short *)(param_1 + 0x250),0x55,(int)&local_b4,
                         ((int)psVar3 - (int)local_b2 >> 1) + 1);
    if (iVar2 != 0) {
                    /* WARNING: Subroutine does not return */
      __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

