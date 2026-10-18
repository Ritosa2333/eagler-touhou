/* Entry: 0x00448d70; symbol: FUN_00448d70; body bytes: 725 */

undefined4 * __fastcall FUN_00448d70(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00422d90(param_1);
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  param_1[7] = 0;
  param_1[8] = 0;
  param_1[9] = 0;
  param_1[10] = 0;
  FUN_00422e10(param_1 + 0xb);
  FUN_00422e10(param_1 + 0xe);
  FUN_00422e10(param_1 + 0x11);
  FUN_004398a0(param_1 + 0x14);
  FUN_004398a0(param_1 + 0x16);
  FUN_004398a0(param_1 + 0x18);
  FUN_004398a0(param_1 + 0x1a);
  FUN_004398a0(param_1 + 0x1c);
  param_1[0x1e] = 0;
  param_1[0x1f] = 0;
  FUN_00422e10(param_1 + 0x20);
  FUN_00447b30(param_1 + 0x23);
  FUN_00447a50(param_1 + 0x38);
  FUN_00447980(param_1 + 0x4d);
  FUN_00447b30(param_1 + 0x58);
  FUN_00447ba0(param_1 + 0x6d);
  FUN_00447ac0(param_1 + 0x78);
  FUN_00447ac0(param_1 + 0x88);
  FUN_00447ac0(param_1 + 0x98);
  FUN_00447a50(param_1 + 0xa8);
  FUN_00447980(param_1 + 0xbd);
  FUN_004479e0(param_1 + 200);
  FUN_004479e0(param_1 + 0xd3);
  FUN_0040bc20(param_1 + 0xde,8,4,FUN_004398a0);
  FUN_004398a0(param_1 + 0xe6);
  param_1[0xe8] = 0;
  param_1[0xe9] = 0;
  param_1[0xea] = 0;
  param_1[0xeb] = 0;
  param_1[0xec] = 0;
  param_1[0xed] = 0;
  FUN_00447dc0(param_1 + 0xee);
  FUN_00447dc0(param_1 + 0xfe);
  param_1[0x10e] = 0;
  param_1[0x10f] = 0;
  *(undefined2 *)(param_1 + 0x110) = 0;
  FUN_00449050(param_1 + 0x111);
  FUN_00422e10(param_1 + 0x121);
  param_1[0x124] = 0;
  param_1[0x125] = 0;
  param_1[0x126] = 0;
  param_1[0x127] = 0;
  param_1[0x128] = 0;
  param_1[0x129] = 0;
  param_1[0x12a] = 0;
  param_1[299] = 0;
  param_1[300] = 0;
  param_1[0x12d] = 0;
  param_1[0x12e] = 0;
  param_1[0x12f] = 0;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

