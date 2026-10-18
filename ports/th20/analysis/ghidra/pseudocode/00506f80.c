/* Entry: 0x00506f80; symbol: FUN_00506f80; body bytes: 122 */

undefined4 * FUN_00506f80(void)

{
  undefined4 *puVar1;
  int *unaff_FS_OFFSET;
  undefined4 *local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a667;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  puVar1 = (undefined4 *)operator_new(0x100);
  local_8 = 0;
  if (puVar1 == (undefined4 *)0x0) {
    local_18 = (undefined4 *)0x0;
  }
  else {
    local_18 = FUN_00507360(puVar1);
  }
  *unaff_FS_OFFSET = local_10;
  return local_18;
}

