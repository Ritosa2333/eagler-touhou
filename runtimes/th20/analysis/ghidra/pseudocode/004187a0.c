/* Entry: 0x004187a0; symbol: FUN_004187a0; body bytes: 129 */

undefined4 * FUN_004187a0(void)

{
  undefined4 *this;
  int *unaff_FS_OFFSET;
  undefined4 *local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567f24;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  this = (undefined4 *)operator_new(0x38);
  local_8 = 0;
  if (this == (undefined4 *)0x0) {
    local_18 = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x38);
    local_18 = FUN_00418a80(this);
  }
  *unaff_FS_OFFSET = local_10;
  return local_18;
}

