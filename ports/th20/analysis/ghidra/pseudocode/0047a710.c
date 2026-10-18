/* Entry: 0x0047a710; symbol: FUN_0047a710; body bytes: 78 */

void * __thiscall FUN_0047a710(void *this,undefined4 param_1)

{
  int *unaff_FS_OFFSET;
  byte local_11;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005679a0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00412e90(this,(uint)local_11,param_1);
  *unaff_FS_OFFSET = local_10;
  return this;
}

