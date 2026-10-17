/* Entry: 0x0053fb00; symbol: FUN_0053fb00; body bytes: 407 */

void __thiscall FUN_0053fb00(void *this,void *param_1,undefined4 *param_2,undefined4 param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 *puVar5;
  int *unaff_FS_OFFSET;
  undefined4 local_20 [3];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056b1ed;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  puVar5 = *(undefined4 **)((int)this + 8);
  if (puVar5 == *(undefined4 **)((int)this + 0xc)) {
    uVar1 = move<>(param_3);
    uVar1 = FUN_0053f8a0(this,(int)param_2,uVar1);
    FUN_005402d0(this,param_1,uVar1);
  }
  else {
    if (param_2 == puVar5) {
      uVar1 = move<>(param_3);
      FUN_0053f810(this,uVar1);
    }
    else {
      uVar1 = FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                            *)this);
      uVar2 = move<>(param_3);
      FUN_0053f7b0(local_20,uVar1,uVar2);
      local_8 = 0;
      FUN_00414b60();
      uVar2 = move<>(puVar5 + -2);
      uVar3 = move<>(puVar5);
      FUN_00413a90(uVar1,uVar3,uVar2);
      *(int *)((int)this + 8) = *(int *)((int)this + 8) + 8;
      FUN_00467080((int)param_2,(int)(puVar5 + -2),puVar5);
      iVar4 = FUN_004fbff0((int)local_20);
      puVar5 = (undefined4 *)move<>(iVar4);
      uVar1 = puVar5[1];
      *param_2 = *puVar5;
      param_2[1] = uVar1;
      local_8 = 0xffffffff;
      FUN_0053fd40(local_20);
    }
    FUN_005402d0(this,param_1,param_2);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

