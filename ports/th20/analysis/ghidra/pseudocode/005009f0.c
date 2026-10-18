/* Entry: 0x005009f0; symbol: FUN_005009f0; body bytes: 608 */

void __fastcall FUN_005009f0(int param_1)

{
  float fVar1;
  bool bVar2;
  int iVar3;
  void *pvVar4;
  int *piVar5;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var6;
  int *piVar7;
  uint uVar8;
  default_delete<class_std::_Facet_base> *pdVar9;
  float *pfVar10;
  int *unaff_FS_OFFSET;
  float10 fVar11;
  float10 fVar12;
  float local_2c;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a61d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_2c = DAT_0056fd50;
  iVar3 = FUN_00412730(*(int *)(param_1 + 0x120));
  pvVar4 = (void *)FUN_004fce40(iVar3);
  FUN_0040c080(local_1c,8);
  FUN_00412280(pvVar4,local_1c);
  local_8 = 0;
  piVar5 = (int *)FUN_00412540();
  while( true ) {
    bVar2 = FUN_00411bb0(local_1c,piVar5);
    if (!bVar2) break;
    p_Var6 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040c300(local_1c);
    piVar7 = (int *)FUN_0040f160(p_Var6);
    iVar3 = FUN_004989f0(*piVar7);
    uVar8 = FUN_00506910(iVar3);
    if ((uVar8 & 0xff) == 0) {
      piVar7 = (int *)FUN_0040f160(p_Var6);
      iVar3 = FUN_004989f0(*piVar7);
      uVar8 = FUN_005068f0(iVar3);
      if ((uVar8 & 0xff) == 0) {
        piVar7 = (int *)FUN_0040f160(p_Var6);
        iVar3 = FUN_004989f0(*piVar7);
        uVar8 = FUN_00506930(iVar3);
        if ((uVar8 & 0xff) == 0) {
          piVar7 = (int *)FUN_0040f160(p_Var6);
          iVar3 = FUN_004989f0(*piVar7);
          uVar8 = FUN_005069e0(iVar3);
          if ((uVar8 & 0xff) == 0) {
            piVar7 = (int *)FUN_0040f160(p_Var6);
            iVar3 = FUN_0047a2c0(*piVar7);
            pdVar9 = std::
                     _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                     ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                   *)(param_1 + 0x50));
            if (*(float *)(iVar3 + 4) - *(float *)(pdVar9 + 4) < local_2c) {
              piVar7 = (int *)FUN_0040f160(p_Var6);
              iVar3 = FUN_0047a2c0(*piVar7);
              pdVar9 = std::
                       _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                       ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                     *)(param_1 + 0x50));
              local_2c = *(float *)(iVar3 + 4) - *(float *)(pdVar9 + 4);
              pvVar4 = (void *)FUN_0040ff90(*(int *)(param_1 + 0x120));
              iVar3 = FUN_00506480(pvVar4,*(uint *)(param_1 + 0xd8));
              piVar7 = (int *)FUN_0040f160(p_Var6);
              pfVar10 = (float *)FUN_0047a2c0(*piVar7);
              pdVar9 = std::
                       _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                       ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                     *)(param_1 + 0x50));
              fVar11 = FUN_004298e0(0x5ba4a8);
              fVar1 = *(float *)(iVar3 + 0x68);
              fVar12 = (float10)FUN_0047a2e0((float *)pdVar9,pfVar10);
              FUN_0047a560((void *)(param_1 + 0x50),(float)fVar12 + (float)fVar11 * fVar1);
            }
          }
        }
      }
    }
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

