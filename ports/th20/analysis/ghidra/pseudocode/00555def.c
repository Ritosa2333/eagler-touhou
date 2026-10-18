/* Entry: 0x00555def; symbol: ___acrt_copy_locale_name; body bytes: 95 */

/* Library Function - Single Match
    ___acrt_copy_locale_name
   
   Library: Visual Studio 2019 Release */

short * __cdecl ___acrt_copy_locale_name(short *param_1)

{
  uint uVar1;
  short *psVar2;
  int iVar3;
  
  if (((param_1 == (short *)0x0) || (uVar1 = FUN_00548430(param_1,0x55), 0x54 < uVar1)) ||
     (psVar2 = (short *)FUN_005584c0(uVar1 * 2 + 2), psVar2 == (short *)0x0)) {
    psVar2 = (short *)0x0;
  }
  else {
    iVar3 = FUN_0055f860(psVar2,uVar1 + 1,(int)param_1,uVar1 + 1);
    if (iVar3 != 0) {
                    /* WARNING: Subroutine does not return */
      __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
  }
  return psVar2;
}

