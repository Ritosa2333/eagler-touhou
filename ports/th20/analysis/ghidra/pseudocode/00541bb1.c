/* Entry: 0x00541bb1; symbol: FUN_00541bb1; body bytes: 113 */

void __cdecl FUN_00541bb1(int param_1)

{
  code *pcVar1;
  int iVar2;
  _Lockit local_c [4];
  int *local_8;
  
  std::_Lockit::_Lockit(local_c,0);
  iVar2 = *(int *)(param_1 + 0xc);
  while (iVar2 != 0) {
    iVar2 = iVar2 + -1;
    local_8 = *(int **)(*(int *)(param_1 + 8) + iVar2 * 4);
    if (local_8 != (int *)0x0) {
      pcVar1 = *(code **)(*local_8 + 8);
      (*(code *)PTR_guard_check_icall_0056c37c)();
      local_8 = (int *)(*pcVar1)();
      if (local_8 != (int *)0x0) {
        pcVar1 = *(code **)*local_8;
        (*(code *)PTR_guard_check_icall_0056c37c)(1);
        (*pcVar1)();
      }
    }
  }
  thunk_FUN_00557ba0(*(LPVOID *)(param_1 + 8));
  FUN_005419ac((int *)local_c);
  return;
}

