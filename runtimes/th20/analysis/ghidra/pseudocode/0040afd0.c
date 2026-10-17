/* Entry: 0x0040afd0; symbol: FUN_0040afd0; body bytes: 150 */

undefined4 * __thiscall FUN_0040afd0(void *this,undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567528;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040b8e0((undefined4 *)this);
  local_8 = 0;
  FUN_0040b8a0((undefined4 *)((int)this + 8));
  local_8 = CONCAT31(local_8._1_3_,1);
  uVar1 = move<>(param_2);
  uVar2 = move<>(param_1);
  FUN_0040b3e0(this,uVar2,uVar1);
  *unaff_FS_OFFSET = local_10;
  return (undefined4 *)this;
}

