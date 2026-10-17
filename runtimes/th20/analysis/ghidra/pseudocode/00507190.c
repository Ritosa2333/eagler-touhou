/* Entry: 0x00507190; symbol: FUN_00507190; body bytes: 135 */

void * FUN_00507190(void)

{
  void *this;
  int *unaff_FS_OFFSET;
  void *local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a767;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  this = operator_new(0x3d87c);
  local_8 = 0;
  if (this == (void *)0x0) {
    local_18 = (void *)0x0;
  }
  else {
    FUN_0040c080(this,0x3d87c);
    local_18 = FUN_00507720(this);
  }
  *unaff_FS_OFFSET = local_10;
  return local_18;
}

