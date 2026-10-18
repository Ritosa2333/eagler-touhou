/* Entry: 0x0054769c; symbol: FUN_0054769c; body bytes: 48 */

void __cdecl FUN_0054769c(int param_1)

{
  code *pcVar1;
  int unaff_FS_OFFSET;
  
  if ((code *)PTR_guard_check_icall_0056c37c != guard_check_icall) {
    if ((*(uint *)(param_1 + 0xc4) < *(uint *)(*(int *)(unaff_FS_OFFSET + 0x18) + 8)) ||
       (*(uint *)(*(int *)(unaff_FS_OFFSET + 0x18) + 4) < *(uint *)(param_1 + 0xc4))) {
      pcVar1 = (code *)swi(0x29);
      (*pcVar1)();
    }
  }
  return;
}

