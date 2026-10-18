/* Entry: 0x004a1410; symbol: FUN_004a1410; body bytes: 125 */

void * __thiscall FUN_004a1410(void *this,undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int *unaff_FS_OFFSET;
  byte local_13;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005674f0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = move<>(param_2);
  uVar2 = move<>(param_1);
  FUN_004a1310(this,(uint)local_13,uVar2,uVar1);
  *unaff_FS_OFFSET = local_10;
  return this;
}

