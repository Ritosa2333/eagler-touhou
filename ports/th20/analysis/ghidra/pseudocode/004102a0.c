/* Entry: 0x004102a0; symbol: FUN_004102a0; body bytes: 149 */

void * FUN_004102a0(void *param_1,int param_2)

{
  undefined4 uVar1;
  int iVar2;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005679a0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  if (param_2 == 0) {
    uVar1 = FUN_00411160();
    FUN_0040de00(param_1,0,uVar1);
  }
  else {
    iVar2 = std::_Winerror_map(param_2);
    if (iVar2 == 0) {
      uVar1 = FUN_00411790();
      FUN_0040de00(param_1,param_2,uVar1);
    }
    else {
      uVar1 = FUN_00411160();
      FUN_0040de00(param_1,iVar2,uVar1);
    }
  }
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

