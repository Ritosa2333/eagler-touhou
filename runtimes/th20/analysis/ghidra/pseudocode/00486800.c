/* Entry: 0x00486800; symbol: FUN_00486800; body bytes: 102 */

void * __thiscall FUN_00486800(void *this,undefined4 param_1)

{
  undefined4 uVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567c8d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  _Func_class<>(this);
  local_8 = 0;
  uVar1 = move<>(param_1);
  FUN_00486a30(this,uVar1);
  *unaff_FS_OFFSET = local_10;
  return this;
}

