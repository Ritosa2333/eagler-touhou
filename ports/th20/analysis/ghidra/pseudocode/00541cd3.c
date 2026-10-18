/* Entry: 0x00541cd3; symbol: __Deletegloballocale; body bytes: 59 */

/* Library Function - Single Match
    __Deletegloballocale
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl __Deletegloballocale(int *param_1)

{
  code *pcVar1;
  undefined4 *puVar2;
  
  if ((int *)*param_1 != (int *)0x0) {
    pcVar1 = *(code **)(*(int *)*param_1 + 8);
    (*(code *)PTR_guard_check_icall_0056c37c)();
    puVar2 = (undefined4 *)(*pcVar1)();
    if (puVar2 != (undefined4 *)0x0) {
      pcVar1 = *(code **)*puVar2;
      (*(code *)PTR_guard_check_icall_0056c37c)(1);
      (*pcVar1)();
    }
  }
  return;
}

