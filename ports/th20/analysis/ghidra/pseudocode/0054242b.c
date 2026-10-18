/* Entry: 0x0054242b; symbol: FUN_0054242b; body bytes: 80 */

void __cdecl FUN_0054242b(LPFILETIME param_1)

{
  code *pcVar1;
  uint uVar2;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  uVar2 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  pcVar1 = DAT_005e4e6c;
  if (DAT_005e4e6c == (code *)0x0) {
    GetSystemTimeAsFileTime(param_1);
  }
  else {
    (*(code *)PTR_guard_check_icall_0056c37c)(param_1,uVar2);
    (*pcVar1)();
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

