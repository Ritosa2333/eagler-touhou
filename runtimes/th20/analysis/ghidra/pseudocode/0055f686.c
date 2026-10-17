/* Entry: 0x0055f686; symbol: ___acrt_execute_initializers; body bytes: 104 */

/* Library Function - Single Match
    ___acrt_execute_initializers
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

undefined4 __cdecl ___acrt_execute_initializers(undefined4 *param_1,undefined4 *param_2)

{
  code *pcVar1;
  undefined4 *in_EAX;
  undefined4 *puVar2;
  
  puVar2 = param_1;
  if (param_1 != param_2) {
    do {
      pcVar1 = (code *)*puVar2;
      if (pcVar1 != (code *)0x0) {
        (*(code *)PTR_guard_check_icall_0056c37c)();
        in_EAX = (undefined4 *)(*pcVar1)();
        if ((char)in_EAX == '\0') break;
      }
      puVar2 = puVar2 + 2;
    } while (puVar2 != param_2);
    if (puVar2 != param_2) {
      if (puVar2 != param_1) {
        puVar2 = puVar2 + -1;
        do {
          if ((puVar2[-1] != 0) && (pcVar1 = (code *)*puVar2, pcVar1 != (code *)0x0)) {
            (*(code *)PTR_guard_check_icall_0056c37c)(0);
            (*pcVar1)();
          }
          in_EAX = puVar2 + -1;
          puVar2 = puVar2 + -2;
        } while (in_EAX != param_1);
      }
      return (uint)in_EAX & 0xffffff00;
    }
  }
  return CONCAT31((int3)((uint)in_EAX >> 8),1);
}

