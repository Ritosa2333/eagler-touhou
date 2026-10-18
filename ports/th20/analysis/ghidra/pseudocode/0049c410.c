/* Entry: 0x0049c410; symbol: FUN_0049c410; body bytes: 86 */

void __thiscall FUN_0049c410(void *this,float param_1)

{
  int *unaff_FS_OFFSET;
  float10 fVar1;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  fVar1 = FUN_00438540(param_1);
  FUN_00452f20((void *)((int)this + 0x28),(float)fVar1);
  *unaff_FS_OFFSET = local_10;
  return;
}

