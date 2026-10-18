/* Entry: 0x0046b6e0; symbol: FUN_0046b6e0; body bytes: 259 */

void __fastcall FUN_0046b6e0(int param_1)

{
  int iVar1;
  shared_ptr<class___ExceptionPtr> *psVar2;
  undefined4 *puVar3;
  int *unaff_FS_OFFSET;
  undefined1 local_54 [4];
  undefined1 local_50 [4];
  void *local_4c;
  int *local_48;
  void *local_44;
  int local_40;
  shared_ptr<class___ExceptionPtr> local_3c [40];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00568d8d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_40 = param_1;
  FUN_0046b7f0(param_1);
  FUN_0040e5e0();
  while( true ) {
    iVar1 = FUN_0040ff90(local_40 + 0x1a234);
    if (iVar1 == 0) break;
    FUN_00414150(local_3c);
    local_8 = 0;
    psVar2 = (shared_ptr<class___ExceptionPtr> *)FUN_00470600((undefined4 *)(local_40 + 0x1a234));
    std::shared_ptr<class___ExceptionPtr>::operator=(local_3c,psVar2);
    local_4c = (void *)(local_40 + 0x1a234);
    local_44 = (void *)(local_40 + 0x1a234);
    puVar3 = (undefined4 *)FUN_00470270(local_44,local_50);
    local_48 = (int *)*puVar3;
    FUN_004705a0(local_4c,local_54,local_48);
    FUN_00414740((int)local_3c);
    local_8 = 0xffffffff;
    FUN_00414400(local_3c);
  }
  FUN_00416140();
  *(int *)(local_40 + 0x1a240) = *(int *)(local_40 + 0x1a240) + 1;
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

