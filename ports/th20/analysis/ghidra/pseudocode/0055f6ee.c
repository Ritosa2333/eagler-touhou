/* Entry: 0x0055f6ee; symbol: ___acrt_execute_uninitializers; body bytes: 49 */

/* Library Function - Single Match
    ___acrt_execute_uninitializers
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

undefined1 __cdecl ___acrt_execute_uninitializers(int param_1,int param_2)

{
  code *pcVar1;
  
  if (param_1 != param_2) {
    do {
      pcVar1 = *(code **)(param_2 + -4);
      if (pcVar1 != (code *)0x0) {
        (*(code *)PTR_guard_check_icall_0056c37c)(0);
        (*pcVar1)();
      }
      param_2 = param_2 + -8;
    } while (param_2 != param_1);
  }
  return 1;
}

