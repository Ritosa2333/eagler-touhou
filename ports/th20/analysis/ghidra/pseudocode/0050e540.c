/* Entry: 0x0050e540; symbol: FUN_0050e540; body bytes: 138 */

undefined4 * __fastcall FUN_0050e540(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  FUN_0040bc20(param_1 + 4,0x7ae8,0x12,FUN_0050af20);
  FUN_0050af20((undefined2 *)(param_1 + 0x22918));
  FUN_0050e6e0((undefined2 *)(param_1 + 0x247d2));
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

