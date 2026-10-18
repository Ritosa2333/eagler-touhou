/* Entry: 0x00479280; symbol: FUN_00479280; body bytes: 102 */

undefined4 * __fastcall FUN_00479280(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00568fad;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00478430(param_1);
  local_8 = 0;
  *param_1 = &PTR_FUN_0056fb98;
  FUN_004792f0(param_1 + 0x2e);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

