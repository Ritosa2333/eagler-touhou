/* Entry: 0x0050e5d0; symbol: FUN_0050e5d0; body bytes: 199 */

undefined4 * __fastcall FUN_0050e5d0(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  undefined4 *local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a923;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = param_1;
  FUN_0050e540(param_1);
  FUN_0050e540(local_14 + 0x24850);
  local_14[0x490a0] = 0;
  _memset(local_14 + 0x490a1,0,0x40);
  FUN_0040b780(local_14 + 0x490b1);
  local_8 = 0;
  FUN_0040bcf0(local_14 + 0x490b1);
  local_18 = 0;
  FUN_0050ab10(local_14,FUN_0050f3b0,&local_18);
  *unaff_FS_OFFSET = local_10;
  return local_14;
}

