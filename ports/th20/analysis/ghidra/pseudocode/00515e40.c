/* Entry: 0x00515e40; symbol: FUN_00515e40; body bytes: 113 */

undefined4 * __thiscall FUN_00515e40(void *this,int param_1,char param_2)

{
  bool bVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056ac3d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00515ce0(this,param_1);
  local_8 = 0;
                    /* WARNING: Load size is inaccurate */
  bVar1 = FUN_0051ac30(*this,param_2);
  *(bool *)((int)this + 4) = bVar1;
  *unaff_FS_OFFSET = local_10;
  return (undefined4 *)this;
}

