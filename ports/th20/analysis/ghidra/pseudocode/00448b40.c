/* Entry: 0x00448b40; symbol: FUN_00448b40; body bytes: 498 */

undefined4 * __fastcall FUN_00448b40(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005682e0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00448d70(param_1);
  FUN_00425cc0(param_1 + 0x130);
  param_1[0x131] = 0;
  FUN_00422d90(param_1 + 0x132);
  FUN_00422d90(param_1 + 0x136);
  param_1[0x13a] = 0;
  FUN_00411970(param_1 + 0x13b,param_1);
  FUN_00411970(param_1 + 0x140,param_1);
  FUN_00411970(param_1 + 0x145,param_1);
  FUN_00411970(param_1 + 0x14a,param_1);
  FUN_00411970(param_1 + 0x14f,param_1);
  param_1[0x154] = 0;
  param_1[0x155] = 0;
  param_1[0x156] = 0;
  param_1[0x157] = 0;
  param_1[0x158] = 0;
  param_1[0x159] = 0;
  param_1[0x15a] = 0;
  param_1[0x15b] = 0;
  param_1[0x15c] = 0;
  param_1[0x15d] = 0;
  *(undefined1 *)(param_1 + 0x15e) = 0;
  *(undefined1 *)((int)param_1 + 0x579) = 0;
  FUN_00447dc0(param_1 + 0x15f);
  FUN_00422e10(param_1 + 0x16f);
  param_1[0x172] = 0;
  param_1[0x173] = 0;
  FUN_00422e10(param_1 + 0x174);
  param_1[0x177] = 0;
  param_1[0x178] = 0;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

