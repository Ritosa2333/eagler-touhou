/* Entry: 0x004f5810; symbol: FUN_004f5810; body bytes: 82 */

void * __fastcall FUN_004f5810(void *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *(int *)((int)param_1 + 0x2c) = *(int *)((int)param_1 + 0x2c) + *(int *)((int)param_1 + 0x34);
  FUN_004fcf50(param_1);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

