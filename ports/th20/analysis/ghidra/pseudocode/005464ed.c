/* Entry: 0x005464ed; symbol: __IsExceptionObjectToBeDestroyed; body bytes: 38 */

/* Library Function - Single Match
    __IsExceptionObjectToBeDestroyed
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

undefined4 __cdecl __IsExceptionObjectToBeDestroyed(int param_1)

{
  int *piVar1;
  int iVar2;
  
  iVar2 = ___vcrt_getptd();
  piVar1 = *(int **)(iVar2 + 0x24);
  while( true ) {
    if (piVar1 == (int *)0x0) {
      return 1;
    }
    if (*piVar1 == param_1) break;
    piVar1 = (int *)piVar1[1];
  }
  return 0;
}

