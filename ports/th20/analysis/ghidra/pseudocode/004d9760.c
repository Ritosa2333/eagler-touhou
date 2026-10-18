/* Entry: 0x004d9760; symbol: FUN_004d9760; body bytes: 132 */

undefined4 __cdecl FUN_004d9760(undefined4 param_1,undefined4 param_2)

{
  void *this;
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00569d10;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  this = (void *)FUN_004d8dd0(0x10);
  local_8 = 0;
  if (this == (void *)0x0) {
    local_18 = 0;
  }
  else {
    FUN_0040c080(this,0x10);
    local_18 = FUN_004d8790(this,param_1,param_2);
  }
  *unaff_FS_OFFSET = local_10;
  return local_18;
}

