/* Entry: 0x00558e20; symbol: fp_format_nan_or_infinity; body bytes: 148 */

/* Library Function - Single Match
    int __cdecl fp_format_nan_or_infinity(enum __acrt_fp_class,bool,char *,unsigned int,bool)
   
   Library: Visual Studio 2019 Release */

int __cdecl
fp_format_nan_or_infinity
          (__acrt_fp_class param_1,bool param_2,char *param_3,uint param_4,bool param_5)

{
  char *pcVar1;
  char cVar2;
  int iVar3;
  char *pcVar4;
  
  if (param_4 < param_2 + 4) {
    *param_3 = '\0';
    return 0xc;
  }
  if (param_2) {
    *param_3 = '-';
    param_3 = param_3 + 1;
    *param_3 = '\0';
    if (param_4 != 0xffffffff) {
      param_4 = param_4 - 1;
    }
  }
  iVar3 = (param_5 ^ 1) * 2;
  pcVar4 = (&PTR_DAT_0059a380)[param_1 * 4 + -4 + iVar3];
  pcVar1 = pcVar4 + 1;
  do {
    cVar2 = *pcVar4;
    pcVar4 = pcVar4 + 1;
  } while (cVar2 != '\0');
  iVar3 = FUN_00548610(param_3,param_4,
                       (int)(&PTR_DAT_0059a380)
                            [((param_1 * 4 + -3) -
                             (uint)((uint)((int)pcVar4 - (int)pcVar1) < param_4)) + iVar3]);
  if (iVar3 == 0) {
    return 0;
  }
                    /* WARNING: Subroutine does not return */
  __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
}

