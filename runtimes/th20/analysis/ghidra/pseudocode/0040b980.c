/* Entry: 0x0040b980; symbol: FUN_0040b980; body bytes: 71 */

void __fastcall FUN_0040b980(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040bcf0(param_1);
  FUN_0040b9d0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

