/* Entry: 0x00441f00; symbol: FUN_00441f00; body bytes: 4347 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_00441f00(void *this,void *param_1)

{
  code *pcVar1;
  int iVar2;
  undefined4 uVar3;
  byte bVar4;
  undefined3 extraout_var;
  float *pfVar5;
  default_delete<class_std::_Facet_base> *pdVar6;
  default_delete<class_std::_Facet_base> *pdVar7;
  uint uVar8;
  int *piVar9;
  undefined4 uVar10;
  int iVar11;
  undefined4 *puVar12;
  undefined4 *puVar13;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var14;
  int local_e8;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_d8 [48];
  float local_a8;
  float fStack_a4;
  float fStack_a0;
  float local_98 [5];
  float fStack_84;
  float local_78;
  float fStack_74;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_58 [64];
  undefined4 local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  std::
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
  _Get_first(local_58);
  std::
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
  _Get_first(local_d8);
  std::
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
  _Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)local_98);
  bVar4 = FUN_00445ab0((int)param_1);
  if (((CONCAT31(extraout_var,bVar4) != 0) && ((*(uint *)((int)param_1 + 0x49c) & 1) != 0)) &&
     (*(char *)((int)param_1 + 0x493) != '\0')) {
    if (*(int *)((int)this + 0x6000e70) != 0) {
      FUN_004455c0((int)this);
    }
    if ((*(uint *)((int)param_1 + 0x49c) >> 4 & 1) == 0) {
      FUN_004ddfe0(0x5c4d40);
    }
    else {
      FUN_004ddf80(0x5c4d40);
    }
    if ((*(uint *)((int)param_1 + 0x49c) >> 0x1a & 1) == 0) {
      puVar12 = (undefined4 *)((int)param_1 + 0x3b8);
      puVar13 = (undefined4 *)((int)param_1 + 0x57c);
      for (iVar11 = 0x10; iVar11 != 0; iVar11 = iVar11 + -1) {
        *puVar13 = *puVar12;
        puVar12 = puVar12 + 1;
        puVar13 = puVar13 + 1;
      }
      *(float *)((int)param_1 + 0x57c) =
           *(float *)((int)param_1 + 0x50) * *(float *)((int)param_1 + 0x58) *
           *(float *)((int)param_1 + 0x57c);
      *(float *)((int)param_1 + 0x590) =
           *(float *)((int)param_1 + 0x54) * *(float *)((int)param_1 + 0x5c) *
           *(float *)((int)param_1 + 0x590);
      *(uint *)((int)param_1 + 0x49c) = *(uint *)((int)param_1 + 0x49c) & 0xfffffffb;
      if ((*(char *)((int)param_1 + 0x4a4) == '\x01') || (*(char *)((int)param_1 + 0x4a4) == '\x05')
         ) {
        *(float *)((int)param_1 + 0x57c) = *(float *)((int)param_1 + 0x57c) * DAT_005b8818;
        *(float *)((int)param_1 + 0x590) = *(float *)((int)param_1 + 0x590) * DAT_005b8818;
      }
      else if ((*(char *)((int)param_1 + 0x4a4) == '\x02') ||
              (*(char *)((int)param_1 + 0x4a4) == '\x06')) {
        *(float *)((int)param_1 + 0x57c) =
             DAT_005b8818 * DAT_0056e0ec * *(float *)((int)param_1 + 0x57c);
        *(float *)((int)param_1 + 0x590) =
             DAT_005b8818 * DAT_0056e0ec * *(float *)((int)param_1 + 0x590);
      }
      pfVar5 = (float *)FUN_00437770((int)param_1);
      local_14 = *pfVar5;
      local_10 = pfVar5[1];
      local_c = pfVar5[2];
      switch(*(byte *)((int)param_1 + 0x4a2) >> 2 & 7) {
      case 0:
        if ((double)local_14 != _DAT_0056cd50) {
          D3DXMatrixRotationX(local_58,local_14);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
        if ((double)local_10 != _DAT_0056cd50) {
          D3DXMatrixRotationY(local_58,local_10);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
        if ((double)local_c != _DAT_0056cd50) {
          D3DXMatrixRotationZ(local_58,local_c);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
        break;
      case 1:
        if ((double)local_14 != _DAT_0056cd50) {
          D3DXMatrixRotationX(local_58,local_14);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
        if ((double)local_c != _DAT_0056cd50) {
          D3DXMatrixRotationZ(local_58,local_c);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
        if ((double)local_10 != _DAT_0056cd50) {
          D3DXMatrixRotationY(local_58,local_10);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
        break;
      case 2:
        if ((double)local_10 != _DAT_0056cd50) {
          D3DXMatrixRotationY(local_58,local_10);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
        if ((double)local_14 != _DAT_0056cd50) {
          D3DXMatrixRotationX(local_58,local_14);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
        if ((double)local_c != _DAT_0056cd50) {
          D3DXMatrixRotationZ(local_58,local_c);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
        break;
      case 3:
        if ((double)local_10 != _DAT_0056cd50) {
          D3DXMatrixRotationY(local_58,local_10);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
        if ((double)local_c != _DAT_0056cd50) {
          D3DXMatrixRotationZ(local_58,local_c);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
        if ((double)local_14 != _DAT_0056cd50) {
          D3DXMatrixRotationX(local_58,local_14);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
        break;
      case 4:
        if ((double)local_c != _DAT_0056cd50) {
          D3DXMatrixRotationZ(local_58,local_c);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
        if ((double)local_14 != _DAT_0056cd50) {
          D3DXMatrixRotationX(local_58,local_14);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
        if ((double)local_10 != _DAT_0056cd50) {
          D3DXMatrixRotationY(local_58,local_10);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
        break;
      case 5:
        if ((double)local_c != _DAT_0056cd50) {
          D3DXMatrixRotationZ(local_58,local_c);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
        if ((double)local_10 != _DAT_0056cd50) {
          D3DXMatrixRotationY(local_58,local_10);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
        if ((double)local_14 != _DAT_0056cd50) {
          D3DXMatrixRotationX(local_58,local_14);
          pdVar7 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          pdVar6 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)param_1 + 0x57c));
          D3DXMatrixMultiply(pdVar6,pdVar7,local_58);
        }
      }
      *(uint *)((int)param_1 + 0x49c) = *(uint *)((int)param_1 + 0x49c) & 0xfffffffd;
    }
    pdVar7 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)((int)param_1 + 0x57c));
    p_Var14 = local_d8;
    for (iVar11 = 0x10; iVar11 != 0; iVar11 = iVar11 + -1) {
      *(undefined4 *)p_Var14 = *(undefined4 *)pdVar7;
      pdVar7 = pdVar7 + 4;
      p_Var14 = p_Var14 + 4;
    }
    local_a8 = (*(float *)((int)param_1 + 0x2c) + *(float *)((int)param_1 + 0x5bc) +
               *(float *)((int)param_1 + 0x484)) -
               *(float *)((int)param_1 + 0x80) * *(float *)((int)param_1 + 0x50) *
               *(float *)((int)param_1 + 0x58);
    fStack_a4 = (*(float *)((int)param_1 + 0x30) + *(float *)((int)param_1 + 0x5c0) +
                *(float *)((int)param_1 + 0x488)) -
                *(float *)((int)param_1 + 0x84) * *(float *)((int)param_1 + 0x54) *
                *(float *)((int)param_1 + 0x5c);
    FUN_004379e0(param_1,&local_a8);
    FUN_00445bc0(this,(int)param_1);
    if ((*(byte *)((int)param_1 + 0x4a1) >> 2 & 7) == 0) {
      local_e8 = *(int *)((int)param_1 + 0x490);
    }
    else {
      local_e8 = *(int *)((int)param_1 + 0x494);
    }
    local_18 = local_e8;
    iVar11 = local_18;
    if (*(int *)((int)this + 0x7d40e90) != 0) {
      local_18._2_1_ = (byte)((uint)local_e8 >> 0x10);
      bVar4 = local_18._2_1_;
      local_18 = iVar11;
      uVar8 = FUN_0043f520(bVar4,*(byte *)((int)this + 0x7d40e8e));
      iVar11 = local_18;
      local_18._0_3_ = CONCAT12((char)uVar8,(undefined2)local_18);
      uVar10 = local_18;
      local_18._1_1_ = SUB41(iVar11,1);
      bVar4 = local_18._1_1_;
      local_18 = uVar10;
      uVar8 = FUN_0043f520(bVar4,*(byte *)((int)this + 0x7d40e8d));
      local_18._0_2_ = CONCAT11((char)uVar8,(byte)local_18);
      uVar8 = FUN_0043f520((byte)local_18,*(byte *)((int)this + 0x7d40e8c));
      uVar10 = local_18;
      local_18 = CONCAT31(local_18._1_3_,(char)uVar8);
      uVar3 = local_18;
      local_18._3_1_ = SUB41(uVar10,3);
      bVar4 = local_18._3_1_;
      local_18 = uVar3;
      uVar8 = FUN_0043f520(bVar4,*(byte *)((int)this + 0x7d40e8f));
      local_18 = CONCAT13((char)uVar8,(undefined3)local_18);
    }
    if (*(int *)((int)this + 0x6000e04) != local_18) {
      FUN_004455c0((int)this);
      *(int *)((int)this + 0x6000e04) = local_18;
      piVar9 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar9 + 0xe4))(piVar9,0x3c,*(undefined4 *)((int)this + 0x6000e04));
    }
    fStack_a0 = *(float *)((int)param_1 + 0x34) + *(float *)((int)param_1 + 0x5c4) +
                *(float *)((int)param_1 + 0x48c);
    piVar9 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar9 + 0xb0))(piVar9,0x100,local_d8);
    iVar11 = FUN_00437cd0((int)param_1);
    if (*(int *)((int)this + 0x6000e08) != *(int *)(iVar11 + 8)) {
      *(undefined4 *)((int)this + 0x6000e08) = *(undefined4 *)(iVar11 + 8);
      FUN_004455c0((int)this);
      piVar9 = (int *)FUN_00412730(0x5c4d40);
      pcVar1 = *(code **)(*piVar9 + 0x104);
      uVar10 = FUN_00445a00(this,*(uint *)((int)this + 0x6000e08));
      (*pcVar1)(piVar9,0,uVar10);
    }
    if ((((*(int *)((int)this + 0x6000e18) != iVar11) ||
         (*(float *)((int)param_1 + 0x78) != _DAT_0056e0e8)) ||
        (*(float *)((int)param_1 + 0x7c) != _DAT_0056e0e8)) ||
       ((*(float *)((int)param_1 + 0x68) != DAT_0056c8cc ||
        (*(float *)((int)param_1 + 0x6c) != DAT_0056c8cc)))) {
      *(int *)((int)this + 0x6000e18) = iVar11;
      pdVar7 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)((int)param_1 + 0x3f8));
      pfVar5 = local_98;
      for (iVar11 = 0x10; iVar11 != 0; iVar11 = iVar11 + -1) {
        *pfVar5 = *(float *)pdVar7;
        pdVar7 = pdVar7 + 4;
        pfVar5 = pfVar5 + 1;
      }
      local_78 = *(float *)((int)param_1 + 0x378) + *(float *)((int)param_1 + 0x78);
      fStack_74 = *(float *)((int)param_1 + 0x37c) + *(float *)((int)param_1 + 0x7c);
      local_98[0] = local_98[0] * *(float *)((int)param_1 + 0x68);
      fStack_84 = fStack_84 * *(float *)((int)param_1 + 0x6c);
      piVar9 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar9 + 0xb0))(piVar9,0x10,local_98);
    }
    if (*(char *)((int)this + 0x6000e0e) != '\x02') {
      piVar9 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar9 + 400))(piVar9,0,*(undefined4 *)((int)this + 0x6000e1c),0,0x14);
      piVar9 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar9 + 0x164))(piVar9,0x102);
      *(undefined1 *)((int)this + 0x6000e0e) = 2;
    }
    FUN_00446690(this,'\x01');
    iVar11 = *(int *)((int)param_1 + 0x4ac);
    iVar2 = *(int *)((int)param_1 + 0x4a8);
    piVar9 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar9 + 0x144))(piVar9,5,(iVar11 * 3 + iVar2) * 4,2);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

