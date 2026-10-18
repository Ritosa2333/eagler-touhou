/* Entry: 0x0045f0a0; symbol: FUN_0045f0a0; body bytes: 156 */

undefined4 * FUN_0045f0a0(void)

{
  void *this;
  undefined4 *puVar1;
  int *unaff_FS_OFFSET;
  undefined4 *local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00568997;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  this = operator_new(0xbac);
  local_8 = 0;
  if (this == (void *)0x0) {
    local_18 = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0xbac);
    puVar1 = (undefined4 *)move<>(&stack0x00000008);
    local_18 = FUN_0045f340(this,(void *)*puVar1);
  }
  *unaff_FS_OFFSET = local_10;
  return local_18;
}

