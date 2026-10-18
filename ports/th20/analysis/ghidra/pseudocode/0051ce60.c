/* Entry: 0x0051ce60; symbol: FUN_0051ce60; body bytes: 135 */

undefined1 __thiscall FUN_0051ce60(void *this,undefined1 param_1)

{
  int *unaff_FS_OFFSET;
  undefined1 local_2c [8];
  void *local_24;
  locale *local_20;
  locale *local_1c;
  void *local_18;
  undefined1 local_11;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056ad8d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  local_18 = this;
  local_20 = (locale *)FUN_0051ba60(this,local_2c);
  local_8 = 0;
  local_1c = local_20;
  local_24 = (void *)FUN_005156d0(local_20);
  local_11 = FUN_0051cef0(local_24,param_1);
  local_8 = 0xffffffff;
  FUN_004f5600((int)local_2c);
  *unaff_FS_OFFSET = local_10;
  return local_11;
}

