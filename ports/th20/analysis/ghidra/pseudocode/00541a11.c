/* Entry: 0x00541a11; symbol: FUN_00541a11; body bytes: 49 */

void __fastcall FUN_00541a11(int param_1)

{
  code *pcVar1;
  undefined4 *puVar2;
  
  pcVar1 = *(code **)(**(int **)(param_1 + 4) + 8);
  (*(code *)PTR_guard_check_icall_0056c37c)();
  puVar2 = (undefined4 *)(*pcVar1)();
  if (puVar2 != (undefined4 *)0x0) {
    pcVar1 = *(code **)*puVar2;
    (*(code *)PTR_guard_check_icall_0056c37c)(1);
    (*pcVar1)();
  }
  return;
}

