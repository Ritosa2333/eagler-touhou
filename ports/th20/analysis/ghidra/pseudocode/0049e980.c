/* Entry: 0x0049e980; symbol: FUN_0049e980; body bytes: 177 */

undefined4 * FUN_0049e980(void)

{
  undefined4 uVar1;
  void *this;
  undefined4 *puVar2;
  int *unaff_FS_OFFSET;
  undefined4 *local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00569537;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  this = operator_new(0xf0);
  local_8 = 0;
  if (this == (void *)0x0) {
    local_18 = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0xf0);
    puVar2 = (undefined4 *)move<>(&stack0x0000000c);
    uVar1 = *puVar2;
    puVar2 = (undefined4 *)move<>(&stack0x00000008);
    local_18 = FUN_0049ecd0(this,*puVar2,uVar1);
  }
  *unaff_FS_OFFSET = local_10;
  return local_18;
}

