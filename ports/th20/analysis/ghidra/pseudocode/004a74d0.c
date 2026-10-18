/* Entry: 0x004a74d0; symbol: FUN_004a74d0; body bytes: 556 */

void __thiscall FUN_004a74d0(void *this,char **param_1)

{
  char cVar1;
  bool bVar2;
  int iVar3;
  undefined1 (*pauVar4) [16];
  int *unaff_FS_OFFSET;
  SIZE_T *pSVar5;
  undefined1 local_90 [32];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_70 [28];
  undefined4 *local_54;
  undefined4 *local_50;
  void *local_4c;
  undefined4 *local_48;
  undefined4 *local_44;
  undefined4 *local_40;
  void *local_3c;
  void *local_38;
  int *local_34;
  undefined *local_30;
  int local_2c;
  void *local_28;
  void *local_24;
  int local_20;
  int local_1c;
  int local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005697d8;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_28 = this;
  local_2c = FUN_00412730(*(int *)((int)this + 0x238));
  local_2c = local_2c + 0xb4;
  local_24 = (void *)FUN_00414f30(local_2c);
  local_38 = (void *)FUN_00414f70(local_2c);
  for (; local_24 != local_38; local_24 = (void *)((int)local_24 + 0x1c)) {
    local_3c = local_24;
    cVar1 = FUN_00446de0(local_24,(uint *)param_1);
    if (cVar1 != '\0') {
      std::_Adl_verify_range<char*,char_const*>((char **)"%s is skiped.\n",param_1);
      goto LAB_004a76e1;
    }
  }
  iVar3 = FUN_00412730(*(int *)((int)local_28 + 0x238));
  local_4c = (void *)(iVar3 + 0xb4);
  local_48 = FUN_0046a830(local_70,(uint *)param_1);
  local_8 = 0;
  local_44 = local_48;
  local_40 = local_48;
  FUN_004a2a20(local_4c,local_48);
  local_8 = 0xffffffff;
  FUN_00449240(local_70);
  local_20 = 0;
  local_30 = &DAT_005c49f4;
  FUN_0040c080(&local_18,4);
  FUN_004a9d10(local_30,&local_18);
  FUN_0040c080(&local_1c,4);
  FUN_004a9d40(local_30,&local_1c);
  do {
    bVar2 = FUN_00449400(&local_18,&local_1c);
    if (bVar2) {
LAB_004a7664:
      if (local_20 == 0) {
        iVar3 = 0;
        pSVar5 = (SIZE_T *)0x0;
        pauVar4 = (undefined1 (*) [16])FUN_004bd4a0((int)param_1);
        local_20 = FUN_00410aa0(pauVar4,pSVar5,iVar3);
        local_54 = FUN_004a1100(local_90,&local_20,&param_1);
        local_8 = 1;
        local_50 = local_54;
        FUN_004a29e0(&DAT_005c49f4,local_54);
        local_8 = 0xffffffff;
        FUN_0046ade0((int)local_90);
      }
      FUN_0053fe60(local_28,local_20);
LAB_004a76e1:
      *unaff_FS_OFFSET = local_10;
      FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
      return;
    }
    local_34 = (int *)FUN_004a3ec0(&local_18);
    cVar1 = FUN_00446de0(local_34 + 1,(uint *)param_1);
    if (cVar1 != '\0') {
      local_20 = *local_34;
      std::_Adl_verify_range<char*,char_const*>((char **)"%s was loaded in 1P.\n",param_1);
      goto LAB_004a7664;
    }
    FUN_0048be00(&local_18);
  } while( true );
}

