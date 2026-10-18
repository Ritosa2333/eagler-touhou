/* Entry: 0x004b9c10; symbol: FUN_004b9c10; body bytes: 473 */

undefined4 * __fastcall FUN_004b9c10(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = 0x200002;
  param_1[1] = 0xb0;
  FUN_0040bc20(param_1 + 2,0x30,2,FUN_0041fb10);
  param_1[0x1a] = 0;
  param_1[0x1b] = 0;
  *(undefined2 *)(param_1 + 0x1c) = 600;
  *(undefined2 *)((int)param_1 + 0x72) = 600;
  *(undefined1 *)(param_1 + 0x1d) = 0;
  *(undefined1 *)((int)param_1 + 0x75) = 1;
  *(undefined1 *)((int)param_1 + 0x76) = 1;
  param_1[0x1e] = 8;
  *(undefined1 *)(param_1 + 0x1f) = 0;
  *(undefined1 *)((int)param_1 + 0x7d) = 2;
  *(undefined1 *)((int)param_1 + 0x7e) = 100;
  *(undefined1 *)((int)param_1 + 0x7f) = 0x50;
  *(undefined1 *)(param_1 + 0x20) = 0;
  *(undefined1 *)((int)param_1 + 0x81) = 2;
  *(undefined1 *)((int)param_1 + 0x82) = 0;
  *(undefined1 *)((int)param_1 + 0x83) = 0;
  FUN_004b9b80(param_1 + 0x21);
  param_1[0x22] = 0x80000000;
  param_1[0x23] = 0x80000000;
  param_1[0x24] = 1;
  *(undefined1 *)(param_1 + 0x25) = 0;
  *(undefined1 *)((int)param_1 + 0x95) = 0;
  *(undefined1 *)((int)param_1 + 0x96) = 0;
  *(undefined1 *)((int)param_1 + 0x97) = 0;
  *(undefined1 *)(param_1 + 0x26) = 0;
  *(undefined1 *)((int)param_1 + 0x99) = 0;
  *(undefined1 *)((int)param_1 + 0x9a) = 1;
  *(undefined1 *)((int)param_1 + 0x9b) = 0;
  *(undefined1 *)(param_1 + 0x27) = 0;
  *(undefined1 *)((int)param_1 + 0x9d) = 0;
  *(undefined1 *)((int)param_1 + 0x9e) = 0;
  *(undefined1 *)((int)param_1 + 0x9f) = 0;
  *(undefined1 *)(param_1 + 0x28) = 0;
  *(undefined1 *)((int)param_1 + 0xa1) = 0;
  *(undefined1 *)((int)param_1 + 0xa2) = 0;
  *(undefined1 *)((int)param_1 + 0xa3) = 0;
  *(undefined1 *)(param_1 + 0x29) = 0;
  param_1[0x2a] = 0;
  param_1[0x2b] = 0;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

