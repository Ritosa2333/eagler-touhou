/* Entry: 0x00464640; symbol: FUN_00464640; body bytes: 156 */

undefined4 * FUN_00464640(void)

{
  void *this;
  undefined4 *puVar1;
  int *unaff_FS_OFFSET;
  undefined4 *local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00568ad7;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  this = operator_new(0x1d88);
  local_8 = 0;
  if (this == (void *)0x0) {
    local_18 = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0x1d88);
    puVar1 = (undefined4 *)move<>(&stack0x00000008);
    local_18 = FUN_004646e0(this,(void *)*puVar1);
  }
  *unaff_FS_OFFSET = local_10;
  return local_18;
}

