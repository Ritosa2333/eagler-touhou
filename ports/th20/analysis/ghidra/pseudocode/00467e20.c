/* Entry: 0x00467e20; symbol: FUN_00467e20; body bytes: 102 */

void * __thiscall FUN_00467e20(void *this,undefined4 param_1)

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
  _Reset<>(this,uVar1);
  *unaff_FS_OFFSET = local_10;
  return this;
}

