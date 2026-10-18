/* Entry: 0x0045ecf0; symbol: FUN_0045ecf0; body bytes: 99 */

void * __thiscall FUN_0045ecf0(void *this,undefined4 param_1)

{
  undefined4 uVar1;
  int *unaff_FS_OFFSET;
  byte local_12;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567750;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = move<>(param_1);
  FUN_0040b190(this,(uint)local_12,uVar1);
  *unaff_FS_OFFSET = local_10;
  return this;
}

