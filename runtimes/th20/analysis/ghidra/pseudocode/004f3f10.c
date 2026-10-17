/* Entry: 0x004f3f10; symbol: FUN_004f3f10; body bytes: 148 */

uint * __thiscall FUN_004f3f10(void *this,undefined4 param_1)

{
  int *unaff_FS_OFFSET;
  uint local_1c;
  undefined4 local_18;
  uint *local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005674f0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = (uint *)this;
  FUN_0040bc20(this,4,1,FUN_004f4ec0);
  local_1c = 0;
  FUN_004f4ec0(&local_1c);
  *local_14 = local_1c;
  local_18 = 1;
  FUN_004ee780(local_14,0,param_1);
  *unaff_FS_OFFSET = local_10;
  return local_14;
}

