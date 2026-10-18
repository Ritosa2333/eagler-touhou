/* Entry: 0x005559fe; symbol: FUN_005559fe; body bytes: 43 */

void __cdecl FUN_005559fe(undefined4 *param_1,undefined4 *param_2)

{
  code *pcVar1;
  
  if (param_1 != param_2) {
    do {
      pcVar1 = (code *)*param_1;
      if (pcVar1 != (code *)0x0) {
        (*(code *)PTR_guard_check_icall_0056c37c)();
        (*pcVar1)();
      }
      param_1 = param_1 + 1;
    } while (param_1 != param_2);
  }
  return;
}

