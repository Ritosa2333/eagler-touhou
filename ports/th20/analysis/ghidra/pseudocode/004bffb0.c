/* Entry: 0x004bffb0; symbol: FUN_004bffb0; body bytes: 359 */

void * __fastcall FUN_004bffb0(void *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005679a0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00411970(param_1,param_1);
  *(undefined4 *)((int)param_1 + 0x14) = 0;
  *(undefined4 *)((int)param_1 + 0x18) = 0;
  *(undefined4 *)((int)param_1 + 0x1c) = 0;
  *(undefined4 *)((int)param_1 + 0x20) = 0;
  FUN_00447de0((undefined4 *)((int)param_1 + 0x24));
  *(undefined4 *)((int)param_1 + 0x28) = 0;
  FUN_004398a0((undefined4 *)((int)param_1 + 0x2c));
  FUN_00478530((undefined4 *)((int)param_1 + 0x34));
  FUN_00422d90((undefined4 *)((int)param_1 + 0x7c));
  FUN_00425cc0((undefined4 *)((int)param_1 + 0x8c));
  *(undefined4 *)((int)param_1 + 0x90) = 0;
  *(undefined4 *)((int)param_1 + 0x94) = 0;
  *(undefined4 *)((int)param_1 + 0x98) = 0;
  *(undefined4 *)((int)param_1 + 0x9c) = 0;
  *(undefined4 *)((int)param_1 + 0xa0) = 0;
  FUN_00425cc0((undefined4 *)((int)param_1 + 0xa4));
  *(undefined4 *)((int)param_1 + 0xa8) = 0;
  *(undefined4 *)((int)param_1 + 0xac) = 0;
  FUN_00425cc0((undefined4 *)((int)param_1 + 0xb0));
  *(undefined4 *)((int)param_1 + 0xb4) = 0;
  *(undefined4 *)((int)param_1 + 0xb8) = 0;
  *(undefined4 *)((int)param_1 + 0xbc) = 0;
  *(undefined4 *)((int)param_1 + 0xc0) = 0;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

