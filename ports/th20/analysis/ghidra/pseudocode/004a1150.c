/* Entry: 0x004a1150; symbol: FUN_004a1150; body bytes: 150 */

void * __thiscall FUN_004a1150(void *this,undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int *unaff_FS_OFFSET;
  byte local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00568060;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = move<>(param_3);
  uVar2 = move<>(param_2);
  uVar3 = move<>(param_1);
  FUN_004a1290(this,(uint)local_14,uVar3,uVar2,uVar1);
  *unaff_FS_OFFSET = local_10;
  return this;
}

