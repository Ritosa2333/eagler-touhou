/* Entry: 0x0046a830; symbol: FUN_0046a830; body bytes: 128 */

undefined4 * __thiscall FUN_0046a830(void *this,uint *param_1)

{
  int iVar1;
  uint uVar2;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00568d2d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00446c10((undefined4 *)this);
  local_8 = 0;
  iVar1 = FUN_00411180((char *)param_1);
  uVar2 = move<>(iVar1);
  FUN_00468030(this,param_1,uVar2);
  *unaff_FS_OFFSET = local_10;
  return (undefined4 *)this;
}

