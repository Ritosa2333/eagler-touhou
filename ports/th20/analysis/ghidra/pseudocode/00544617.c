/* Entry: 0x00544617; symbol: __CreateFrameInfo; body bytes: 36 */

/* Library Function - Single Match
    __CreateFrameInfo
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

undefined4 * __cdecl __CreateFrameInfo(undefined4 *param_1,undefined4 param_2)

{
  int iVar1;
  
  *param_1 = param_2;
  iVar1 = ___vcrt_getptd();
  param_1[1] = *(undefined4 *)(iVar1 + 0x24);
  iVar1 = ___vcrt_getptd();
  *(undefined4 **)(iVar1 + 0x24) = param_1;
  return param_1;
}

