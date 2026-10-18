/* Entry: 0x0047c2a0; symbol: FUN_0047c2a0; body bytes: 78 */

void __fastcall FUN_0047c2a0(undefined4 param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00542f23(param_1,0x528,0x7d1,FUN_0047c470);
  *unaff_FS_OFFSET = local_10;
  return;
}

