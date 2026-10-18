/* Entry: 0x004a5040; symbol: FUN_004a5040; body bytes: 734 */

void __fastcall FUN_004a5040(int param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var5;
  int *piVar6;
  void *pvVar7;
  int *unaff_FS_OFFSET;
  float10 fVar8;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056974d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_004776a0(DAT_005c0028,*(undefined4 *)(param_1 + 300));
  *(undefined4 *)(param_1 + 0xac) = 0;
  *(undefined4 *)(param_1 + 0xb0) = 0;
  if (0 < *(int *)(param_1 + 0xcc)) {
    bVar1 = FUN_00461070((void *)(param_1 + 0xd0),0);
    if (bVar1) {
      FUN_00429420((void *)(param_1 + 0xd0));
      bVar1 = FUN_00423590((void *)(param_1 + 0xd0),0);
      if (bVar1) {
        *(undefined4 *)(param_1 + 0xcc) = 0;
        FUN_00423520((void *)(param_1 + 0xd0),0);
      }
    }
    iVar2 = FUN_00472040((void *)(param_1 + 0xd0),0x3c);
    iVar3 = FUN_004a3f00((void *)(param_1 + 0xd0),0x3c);
    FUN_004ab5b0(DAT_005c06a4,iVar3,(iVar2 * 100) / 0x3c);
  }
  FUN_0040c080(local_1c,8);
  FUN_00412280((void *)(param_1 + 0x108),local_1c);
  local_8 = 0;
  piVar4 = (int *)FUN_00412540();
  do {
    bVar1 = FUN_00411bb0(local_1c,piVar4);
    if (!bVar1) {
      local_8 = 0xffffffff;
      FUN_00411b00(local_1c);
      FUN_00513dd0();
      FUN_005120d0();
      iVar2 = FUN_0040ff90(*(int *)(param_1 + 0x130));
      fVar8 = FUN_004aaa40(iVar2);
      if ((float)fVar8 <= DAT_0056e730) {
        pvVar7 = (void *)FUN_0040ff90(*(int *)(param_1 + 0x130));
        FUN_004ab850(pvVar7,0);
      }
      else {
        pvVar7 = (void *)FUN_0040ff90(*(int *)(param_1 + 0x130));
        FUN_004ab850(pvVar7,1);
      }
      pvVar7 = (void *)FUN_0040ff90(*(int *)(param_1 + 0x130));
      FUN_004ab590(pvVar7,DAT_0056c8cc);
      FUN_00423540((undefined4 *)(param_1 + 0x9c));
      *unaff_FS_OFFSET = local_10;
      FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
      return;
    }
    p_Var5 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040c300(local_1c);
    piVar6 = (int *)FUN_0040f160(p_Var5);
    if ((*(uint *)(*piVar6 + 0x354) >> 9 & 1) == 0) {
      piVar6 = (int *)FUN_0040f160(p_Var5);
      iVar2 = FUN_004a8760(*piVar6);
      if (iVar2 != 0) goto LAB_004a51fd;
      piVar6 = (int *)FUN_0040f160(p_Var5);
      *(uint *)(*piVar6 + 0x354) = *(uint *)(*piVar6 + 0x354) & 0xfffffffb;
    }
    else {
LAB_004a51fd:
      piVar6 = (int *)FUN_0040f160(p_Var5);
      FUN_004a2720(piVar6);
    }
    FUN_00411c30(local_1c);
  } while( true );
}

