/* Entry: 0x00416140; symbol: FUN_00416140; body bytes: 254 */

void FUN_00416140(void)

{
  int iVar1;
  undefined4 uVar2;
  shared_ptr<class___ExceptionPtr> *psVar3;
  undefined4 *puVar4;
  int *unaff_FS_OFFSET;
  undefined1 local_50 [4];
  undefined1 local_4c [4];
  int *local_48;
  shared_ptr<class___ExceptionPtr> local_44 [40];
  undefined4 local_1c;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  int local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567e2d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  iVar1 = FUN_00412730(0x5b66e0);
  if (iVar1 != 0) {
    uVar2 = FUN_0040c310(&DAT_005c0240,0x12);
    FUN_0040b730(&local_1c,uVar2);
    local_8 = 0;
    uVar2 = FUN_0040c310(&DAT_005c0240,0x13);
    FUN_0040b730(&local_18,uVar2);
    local_8._0_1_ = 1;
    FUN_00414150(local_44);
    local_8._0_1_ = 2;
    psVar3 = (shared_ptr<class___ExceptionPtr> *)FUN_00415600(0x5b66e0);
    std::shared_ptr<class___ExceptionPtr>::operator=(local_44,psVar3);
    puVar4 = (undefined4 *)FUN_004150b0(&DAT_005b66e0,local_4c);
    local_48 = (int *)*puVar4;
    FUN_00415550(&DAT_005b66e0,local_50,local_48);
    FUN_00414740((int)local_44);
    local_8._0_1_ = 1;
    FUN_00414400(local_44);
    local_8 = (uint)local_8._1_3_ << 8;
    FUN_0040b900(&local_18);
    local_8 = 0xffffffff;
    FUN_0040b900(&local_1c);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

