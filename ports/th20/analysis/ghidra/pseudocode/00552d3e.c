/* Entry: 0x00552d3e; symbol: FUN_00552d3e; body bytes: 125 */

void __cdecl FUN_00552d3e(undefined4 param_1)

{
  uint uVar1;
  BOOL BVar2;
  FARPROC pFVar3;
  int *unaff_FS_OFFSET;
  HMODULE local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005679a0;
  local_10 = *unaff_FS_OFFSET;
  uVar1 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = (HMODULE)0x0;
  BVar2 = GetModuleHandleExW(0,L"mscoree.dll",&local_14);
  if (BVar2 != 0) {
    pFVar3 = GetProcAddress(local_14,"CorExitProcess");
    if (pFVar3 != (FARPROC)0x0) {
      (*(code *)PTR_guard_check_icall_0056c37c)(param_1,uVar1);
      (*pFVar3)();
    }
  }
  if (local_14 != (HMODULE)0x0) {
    FreeLibrary(local_14);
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

