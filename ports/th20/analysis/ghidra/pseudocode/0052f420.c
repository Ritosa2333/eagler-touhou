/* Entry: 0x0052f420; symbol: FUN_0052f420; body bytes: 123 */

void * FUN_0052f420(void *param_1)

{
  int *unaff_FS_OFFSET;
  undefined1 auStack_40 [12];
  undefined1 auStack_34 [12];
  uint uStack_28;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00568060;
  local_10 = *unaff_FS_OFFSET;
  uStack_28 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  _String_const_iterator<>(auStack_40,(_Iterator_base12 *)&stack0x00000008);
  FUN_0052d4c0(auStack_34);
  _String_const_iterator<>(auStack_40,(_Iterator_base12 *)&stack0x00000008);
  FUN_0052f190(param_1);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

