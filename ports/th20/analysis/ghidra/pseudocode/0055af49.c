/* Entry: 0x0055af49; symbol: __ctrandisp2; body bytes: 65 */

/* Library Function - Single Match
    __ctrandisp2
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void __cdecl __ctrandisp2(uint param_1,int param_2,uint param_3,int param_4)

{
  undefined4 extraout_ECX;
  int extraout_EDX;
  
  __fload(param_1,param_2);
  __fload(param_3,param_4);
  __trandisp2(extraout_ECX,extraout_EDX);
  FUN_0055af8a();
  return;
}

