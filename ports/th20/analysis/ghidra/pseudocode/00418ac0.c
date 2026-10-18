/* Entry: 0x00418ac0; symbol: FUN_00418ac0; body bytes: 741 */

undefined4 * __fastcall FUN_00418ac0(undefined4 *param_1)

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
  param_1[3] = 0;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  param_1[7] = 0;
  param_1[8] = 0;
  param_1[9] = 0;
  param_1[10] = 0;
  param_1[0xb] = 0;
  param_1[0xc] = 0;
  param_1[0xd] = 0;
  param_1[0xe] = 0;
  param_1[0xf] = 0;
  param_1[0x10] = 0;
  param_1[0x11] = 0;
  param_1[0x12] = 0;
  param_1[0x13] = 0;
  param_1[0x14] = 0;
  param_1[0x15] = 0;
  param_1[0x16] = 0;
  param_1[0x17] = 0;
  param_1[0x18] = 0;
  param_1[0x19] = 0;
  *(undefined1 *)(param_1 + 0x1a) = 0;
  *(undefined1 *)((int)param_1 + 0x69) = 0;
  param_1[0x1b] = 0;
  *(undefined1 *)(param_1 + 0x1c) = 0;
  param_1[0x1e] = 0;
  param_1[0x1f] = 0;
  param_1[0x20] = 0;
  param_1[0x21] = 0;
  *(undefined1 *)(param_1 + 0x22) = 0;
  _memset((void *)((int)param_1 + 0x89),0,0x1000);
  _memset((void *)((int)param_1 + 0x1089),0,0x1000);
  *(undefined1 *)((int)param_1 + 0x2089) = 0;
  *(undefined1 *)((int)param_1 + 0x208a) = 0;
  *(undefined1 *)((int)param_1 + 0x208b) = 0;
  param_1[0x823] = 2;
  FUN_00418dd0(param_1 + 0x824);
  param_1[0x825] = 0;
  *(undefined8 *)(param_1 + 0x826) = 0;
  param_1[0x828] = 0;
  param_1[0x829] = 0;
  param_1[0x82a] = 0;
  param_1[0x82b] = 0;
  param_1[0x82c] = 0;
  param_1[0x82d] = 0;
  param_1[0x82e] = 0;
  param_1[0x82f] = 0;
  param_1[0x830] = 0;
  *(undefined8 *)(param_1 + 0x832) = 0;
  *(undefined8 *)(param_1 + 0x834) = 0;
  *(undefined8 *)(param_1 + 0x836) = 0;
  *(undefined8 *)(param_1 + 0x838) = 0;
  *(undefined8 *)(param_1 + 0x83a) = 0;
  *(undefined8 *)(param_1 + 0x83c) = 0;
  param_1[0x83e] = 0;
  param_1[0x83f] = 0;
  param_1[0x840] = 0;
  FUN_0040bc20(param_1 + 0x841,0xc,4,FUN_00414090);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

