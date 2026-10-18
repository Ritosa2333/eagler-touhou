/* Entry: 0x0050aa60; symbol: FUN_0050aa60; body bytes: 125 */

undefined4 * __thiscall FUN_0050aa60(void *this,undefined4 param_1)

{
  undefined4 uVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a828;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040b8e0((undefined4 *)this);
  local_8 = 0;
  FUN_0040b8a0((undefined4 *)((int)this + 8));
  local_8 = CONCAT31(local_8._1_3_,1);
  uVar1 = move<>(param_1);
  FUN_0050ac70(this,uVar1);
  *unaff_FS_OFFSET = local_10;
  return (undefined4 *)this;
}

