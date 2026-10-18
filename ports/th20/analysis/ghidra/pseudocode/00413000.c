/* Entry: 0x00413000; symbol: FUN_00413000; body bytes: 158 */

undefined4 * __thiscall FUN_00413000(void *this,undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567cbd;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00413f50(this,param_1);
  local_8 = 0;
  FUN_004148d0((undefined4 *)this);
  uVar1 = move<>(param_2);
  uVar2 = move<>(*(int *)((int)this + 4) + 8);
                    /* WARNING: Load size is inaccurate */
  FUN_00413b00(*this,uVar2,uVar1);
  *unaff_FS_OFFSET = local_10;
  return (undefined4 *)this;
}

