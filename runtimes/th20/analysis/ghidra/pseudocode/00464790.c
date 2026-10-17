/* Entry: 0x00464790; symbol: FUN_00464790; body bytes: 100 */

void __fastcall FUN_00464790(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00449370((int)(param_1 + 0x5e6));
  FUN_00542f23(param_1 + 2,0x5e4,4,FUN_00449370);
  FUN_00446ba0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

