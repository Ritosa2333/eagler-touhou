/* Entry: 0x004f51c0; symbol: FUN_004f51c0; body bytes: 83 */

void __fastcall FUN_004f51c0(undefined4 *param_1)

{
  int iVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR_FUN_00572ca8;
  iVar1 = FUN_00412730((int)param_1);
  if (iVar1 != 0) {
    FUN_004fbeb0((int)param_1);
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

