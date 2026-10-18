/* Entry: 0x00556090; symbol: __copytlocinfo_nolock; body bytes: 47 */

/* Library Function - Single Match
    __copytlocinfo_nolock
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl __copytlocinfo_nolock(undefined4 *param_1,undefined4 *param_2)

{
  int iVar1;
  undefined4 *puVar2;
  
  if (((param_2 != (undefined4 *)0x0) && (param_1 != (undefined4 *)0x0)) && (param_1 != param_2)) {
    puVar2 = param_1;
    for (iVar1 = 0x2e; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar2 = *param_2;
      param_2 = param_2 + 1;
      puVar2 = puVar2 + 1;
    }
    param_1[3] = 0;
    FUN_005608bf((int)param_1);
  }
  return;
}

