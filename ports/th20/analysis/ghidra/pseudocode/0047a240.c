/* Entry: 0x0047a240; symbol: FUN_0047a240; body bytes: 78 */

void __thiscall FUN_0047a240(void *this,float param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_004530c0((void *)((int)this + 0x1c),param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

