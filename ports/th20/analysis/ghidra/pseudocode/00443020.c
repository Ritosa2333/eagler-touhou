/* Entry: 0x00443020; symbol: FUN_00443020; body bytes: 2136 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_00443020(void *this,int param_1,undefined4 param_2,int param_3)

{
  code *pcVar1;
  byte bVar2;
  undefined3 extraout_var;
  default_delete<class_std::_Facet_base> *pdVar3;
  default_delete<class_std::_Facet_base> *pdVar4;
  int *piVar5;
  undefined4 uVar6;
  int iVar7;
  undefined4 *puVar8;
  float *pfVar9;
  undefined4 *puVar10;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var11;
  float *pfVar12;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_c8 [64];
  float local_88 [5];
  float fStack_74;
  float local_68;
  float fStack_64;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_48 [48];
  float local_18;
  float fStack_14;
  float fStack_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  std::
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
  _Get_first(local_c8);
  std::
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
  _Get_first(local_48);
  bVar2 = FUN_00445ab0(param_1);
  if (((CONCAT31(extraout_var,bVar2) != 0) && ((*(uint *)(param_1 + 0x49c) & 1) != 0)) &&
     (2 < param_3)) {
    if (*(int *)((int)this + 0x6000e70) != 0) {
      FUN_004455c0((int)this);
    }
    if ((*(uint *)(param_1 + 0x49c) >> 4 & 1) == 0) {
      FUN_004ddfe0(0x5c4d40);
    }
    else {
      FUN_004ddf80(0x5c4d40);
    }
    puVar8 = &DAT_0056e100;
    puVar10 = (undefined4 *)(param_1 + 0x3b8);
    for (iVar7 = 0x10; iVar7 != 0; iVar7 = iVar7 + -1) {
      *puVar10 = *puVar8;
      puVar8 = puVar8 + 1;
      puVar10 = puVar10 + 1;
    }
    puVar8 = (undefined4 *)(param_1 + 0x3b8);
    puVar10 = (undefined4 *)(param_1 + 0x57c);
    for (iVar7 = 0x10; iVar7 != 0; iVar7 = iVar7 + -1) {
      *puVar10 = *puVar8;
      puVar8 = puVar8 + 1;
      puVar10 = puVar10 + 1;
    }
    *(float *)(param_1 + 0x57c) =
         *(float *)(param_1 + 0x50) * *(float *)(param_1 + 0x58) * *(float *)(param_1 + 0x57c);
    *(float *)(param_1 + 0x590) =
         *(float *)(param_1 + 0x54) * *(float *)(param_1 + 0x5c) * *(float *)(param_1 + 0x590);
    *(uint *)(param_1 + 0x49c) = *(uint *)(param_1 + 0x49c) & 0xfffffffb;
    if ((double)*(float *)(param_1 + 0x38) != _DAT_0056cd50) {
      D3DXMatrixRotationX(local_c8,*(undefined4 *)(param_1 + 0x38));
      pdVar3 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(param_1 + 0x57c));
      pdVar4 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(param_1 + 0x57c));
      D3DXMatrixMultiply(pdVar4,pdVar3,local_c8);
    }
    if ((double)*(float *)(param_1 + 0x3c) != _DAT_0056cd50) {
      D3DXMatrixRotationY(local_c8,*(undefined4 *)(param_1 + 0x3c));
      pdVar3 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(param_1 + 0x57c));
      pdVar4 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(param_1 + 0x57c));
      D3DXMatrixMultiply(pdVar4,pdVar3,local_c8);
    }
    if ((double)*(float *)(param_1 + 0x40) != _DAT_0056cd50) {
      D3DXMatrixRotationZ(local_c8,*(undefined4 *)(param_1 + 0x40));
      pdVar3 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(param_1 + 0x57c));
      pdVar4 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(param_1 + 0x57c));
      D3DXMatrixMultiply(pdVar4,pdVar3,local_c8);
    }
    *(uint *)(param_1 + 0x49c) = *(uint *)(param_1 + 0x49c) & 0xfffffffd;
    puVar8 = (undefined4 *)(param_1 + 0x57c);
    p_Var11 = local_48;
    for (iVar7 = 0x10; iVar7 != 0; iVar7 = iVar7 + -1) {
      *(undefined4 *)p_Var11 = *puVar8;
      puVar8 = puVar8 + 1;
      p_Var11 = p_Var11 + 4;
    }
    local_18 = *(float *)(param_1 + 0x2c) + *(float *)(param_1 + 0x5bc) +
               *(float *)(param_1 + 0x484);
    if (((*(byte *)(param_1 + 0x4a3) & 3) == 0) && (*(int *)(param_1 + 0x55c) == 0)) {
      iVar7 = FUN_0041cad0(0x5b6758);
      local_18 = local_18 + (float)iVar7;
      FUN_0041caf0(0x5b6758);
    }
    fStack_14 = *(float *)(param_1 + 0x30) + *(float *)(param_1 + 0x5c0) +
                *(float *)(param_1 + 0x488);
    fStack_10 = *(float *)(param_1 + 0x34) + *(float *)(param_1 + 0x5c4) +
                *(float *)(param_1 + 0x48c);
    piVar5 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar5 + 0xb0))(piVar5,0x100,local_48);
    FUN_00445bc0(this,param_1);
    iVar7 = FUN_00437cd0(param_1);
    if (*(int *)((int)this + 0x6000e08) != *(int *)(iVar7 + 8)) {
      *(undefined4 *)((int)this + 0x6000e08) = *(undefined4 *)(iVar7 + 8);
      piVar5 = (int *)FUN_00412730(0x5c4d40);
      pcVar1 = *(code **)(*piVar5 + 0x104);
      uVar6 = FUN_00445a00(this,*(uint *)((int)this + 0x6000e08));
      (*pcVar1)(piVar5,0,uVar6);
    }
    if (((*(int *)((int)this + 0x6000e18) != iVar7) || (*(float *)(param_1 + 0x78) != _DAT_0056e0e8)
        ) || ((*(float *)(param_1 + 0x7c) != _DAT_0056e0e8 ||
              ((*(float *)(param_1 + 0x68) != DAT_0056c8cc ||
               (*(float *)(param_1 + 0x6c) != DAT_0056c8cc)))))) {
      *(int *)((int)this + 0x6000e18) = iVar7;
      pfVar9 = (float *)(param_1 + 0x3f8);
      pfVar12 = local_88;
      for (iVar7 = 0x10; iVar7 != 0; iVar7 = iVar7 + -1) {
        *pfVar12 = *pfVar9;
        pfVar9 = pfVar9 + 1;
        pfVar12 = pfVar12 + 1;
      }
      local_68 = *(float *)(param_1 + 0x378) + *(float *)(param_1 + 0x78);
      fStack_64 = *(float *)(param_1 + 0x37c) + *(float *)(param_1 + 0x7c);
      local_88[0] = local_88[0] * *(float *)(param_1 + 0x68);
      fStack_74 = fStack_74 * *(float *)(param_1 + 0x6c);
      piVar5 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar5 + 0xb0))(piVar5,0x10,local_88);
    }
    FUN_00446690(this,'\x01');
    if (*(char *)((int)this + 0x6000e0e) != '\x05') {
      piVar5 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar5 + 0x164))(piVar5,0x142);
      piVar5 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar5 + 0x10c))(piVar5,0,6,0);
      piVar5 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar5 + 0x10c))(piVar5,0,3,0);
      *(undefined1 *)((int)this + 0x6000e0e) = 5;
    }
    piVar5 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar5 + 0x14c))(piVar5,5,param_3 + -2,param_2,0x18);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

