/* Entry: 0x00556495; symbol: __wcscats; body bytes: 64 */

/* Library Function - Single Match
    __wcscats
   
   Libraries: Visual Studio 2012 Release, Visual Studio 2015 Release, Visual Studio 2017 Release,
   Visual Studio 2019 Release */

void __cdecl __wcscats(short *param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  
  iVar2 = 0;
  if (0 < param_3) {
    piVar3 = &param_3;
    do {
      piVar3 = piVar3 + 1;
      iVar1 = FUN_005517b0(param_1,param_2,*piVar3);
      if (iVar1 != 0) {
                    /* WARNING: Subroutine does not return */
        __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < param_3);
  }
  return;
}

