/* Entry: 0x00411b00; symbol: FUN_00411b00; body bytes: 111 */

void __fastcall FUN_00411b00(int *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567750;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  if (*param_1 != 0) {
    FUN_00412da0((void *)*param_1,0);
  }
  if (param_1[1] != 0) {
    FUN_00412da0((void *)param_1[1],0);
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

