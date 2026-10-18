/* Entry: 0x00555e81; symbol: ___lc_lctowcs; body bytes: 111 */

/* Library Function - Single Match
    ___lc_lctowcs
   
   Libraries: Visual Studio 2012 Release, Visual Studio 2015 Release, Visual Studio 2017 Release,
   Visual Studio 2019 Release */

void __cdecl ___lc_lctowcs(short *param_1,int param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_00548100(param_1,param_2,param_3);
  if (iVar1 == 0) {
    if (*(short *)(param_3 + 0x80) != 0) {
      __wcscats(param_1,param_2,2);
    }
    if (*(short *)(param_3 + 0x100) != 0) {
      __wcscats(param_1,param_2,2);
    }
    return;
  }
                    /* WARNING: Subroutine does not return */
  __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
}

