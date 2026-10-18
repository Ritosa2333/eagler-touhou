/* Entry: 0x005140b0; symbol: FUN_005140b0; body bytes: 96 */

void __fastcall FUN_005140b0(int param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  puStack_c = &LAB_0056aa9d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  local_8 = 0;
  std::shared_ptr<class___ExceptionPtr>::operator=
            ((shared_ptr<class___ExceptionPtr> *)(param_1 + 0x3f8),
             (shared_ptr<class___ExceptionPtr> *)&stack0x00000004);
  local_8 = 0xffffffff;
  FUN_00414400(&stack0x00000004);
  *unaff_FS_OFFSET = local_10;
  return;
}

