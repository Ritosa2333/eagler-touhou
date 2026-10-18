/* Entry: 0x004ae930; symbol: FUN_004ae930; body bytes: 156 */

undefined4 * FUN_004ae930(void)

{
  void *this;
  undefined4 *puVar1;
  int *unaff_FS_OFFSET;
  undefined4 *local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00569867;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  this = operator_new(0x140);
  local_8 = 0;
  if (this == (void *)0x0) {
    local_18 = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x140);
    puVar1 = (undefined4 *)move<>(&stack0x00000008);
    local_18 = FUN_004af310(this,*puVar1);
  }
  *unaff_FS_OFFSET = local_10;
  return local_18;
}

