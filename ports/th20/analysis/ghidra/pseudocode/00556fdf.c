/* Entry: 0x00556fdf; symbol: parse_bcp47_language; body bytes: 113 */

/* Library Function - Single Match
    _parse_bcp47_language
   
   Library: Visual Studio 2019 Release */

uint __cdecl parse_bcp47_language(short *param_1,int *param_2)

{
  uint in_EAX;
  int iVar1;
  
  if (param_2[2] == 0) {
    in_EAX = param_2[1] - 2;
    if ((in_EAX < 2) && (in_EAX = FUN_0055717f(*param_2,param_2[1]), (char)in_EAX != '\0')) {
      iVar1 = FUN_0055f860(param_1,0x40,*param_2,param_2[1]);
      if ((iVar1 == 0) &&
         (iVar1 = FUN_0055f860(param_1 + 0x90,0x55,*param_2,param_2[1]), iVar1 == 0)) {
        return 1;
      }
                    /* WARNING: Subroutine does not return */
      __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
  }
  return in_EAX & 0xffffff00;
}

