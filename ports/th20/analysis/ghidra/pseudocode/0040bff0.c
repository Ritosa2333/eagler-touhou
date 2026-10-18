/* Entry: 0x0040bff0; symbol: FUN_0040bff0; body bytes: 88 */

void __fastcall FUN_0040bff0(undefined4 *param_1)

{
  bool bVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  bVar1 = FUN_0040c3f0((int)param_1);
  if (bVar1) {
    FUN_0040c510(param_1 + 2);
    FUN_0040c360(param_1);
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

