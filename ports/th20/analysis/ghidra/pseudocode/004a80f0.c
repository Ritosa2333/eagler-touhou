/* Entry: 0x004a80f0; symbol: FUN_004a80f0; body bytes: 362 */

void FUN_004a80f0(void)

{
  bool bVar1;
  int iVar2;
  undefined4 *puVar3;
  int *unaff_FS_OFFSET;
  undefined1 local_3c [4];
  void *local_38;
  undefined4 *local_34;
  undefined4 *local_30;
  int *local_2c;
  void *local_28;
  int local_24;
  int local_20;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056867d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_24 = 0;
  while( true ) {
    iVar2 = FUN_004ab9f0();
    if (iVar2 <= local_24) break;
    FUN_004863f0(DAT_005c0028,local_24 + 0x19,'\0');
    local_24 = local_24 + 1;
  }
  local_28 = (void *)(local_20 + 0x108);
  FUN_0040c080(local_1c,8);
  FUN_00412280(local_28,local_1c);
  local_8 = 0;
  local_2c = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_1c,local_2c);
    if (!bVar1) break;
    local_30 = (undefined4 *)FUN_0040c300(local_1c);
    local_34 = (undefined4 *)FUN_0040c300(local_30);
    FUN_0041f7c0(local_34);
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  local_38 = (void *)(local_20 + 0x54);
  puVar3 = FUN_004142a0(local_3c,0);
  FUN_004aa960(local_38,puVar3);
  FUN_004aa770((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(local_20 + 0xb4));
  FUN_00421780(local_20);
  *(undefined4 *)(local_20 + 0xcc) = 0;
  *(undefined4 *)(local_20 + 0x44) = 0;
  *(undefined4 *)(local_20 + 0x40) = 0;
  FUN_004ab1b0((undefined4 *)(local_20 + 0x10));
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

