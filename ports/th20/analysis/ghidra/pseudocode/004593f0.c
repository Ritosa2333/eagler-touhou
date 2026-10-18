/* Entry: 0x004593f0; symbol: FUN_004593f0; body bytes: 219 */

undefined4 * FUN_004593f0(void)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  void *this;
  undefined4 *puVar4;
  int *unaff_FS_OFFSET;
  undefined4 *local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00568817;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  this = operator_new(0xa8);
  local_8 = 0;
  if (this == (void *)0x0) {
    local_18 = (undefined4 *)0x0;
  }
  else {
    FUN_0040c080(this,0xa8);
    puVar4 = (undefined4 *)move<>(&stack0x00000014);
    uVar1 = *puVar4;
    puVar4 = (undefined4 *)move<>(&stack0x00000010);
    uVar2 = *puVar4;
    puVar4 = (undefined4 *)move<>(&stack0x0000000c);
    uVar3 = *puVar4;
    puVar4 = (undefined4 *)move<>(&stack0x00000008);
    local_18 = FUN_00459630(this,*puVar4,uVar3,uVar2,uVar1);
  }
  *unaff_FS_OFFSET = local_10;
  return local_18;
}

