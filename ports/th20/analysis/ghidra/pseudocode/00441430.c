/* Entry: 0x00441430; symbol: FUN_00441430; body bytes: 1996 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_00441430(void *this,int param_1)

{
  float *pfVar1;
  int iVar2;
  undefined4 *puVar3;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var4;
  undefined4 *puVar5;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_c4 [64];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_84 [48];
  float local_54;
  float fStack_50;
  float fStack_4c;
  float local_44;
  float local_40;
  float local_3c;
  undefined4 local_38;
  undefined4 uStack_34;
  undefined4 uStack_30;
  undefined4 uStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  std::
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
  _Get_first(local_84);
  std::
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
  _Get_first(local_c4);
  if ((*(uint *)(param_1 + 0x49c) >> 0x1a & 1) == 0) {
    puVar3 = (undefined4 *)(param_1 + 0x3b8);
    puVar5 = (undefined4 *)(param_1 + 0x57c);
    for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
      *puVar5 = *puVar3;
      puVar3 = puVar3 + 1;
      puVar5 = puVar5 + 1;
    }
    *(float *)(param_1 + 0x57c) =
         *(float *)(param_1 + 0x50) * *(float *)(param_1 + 0x58) * *(float *)(param_1 + 0x57c);
    *(float *)(param_1 + 0x590) =
         *(float *)(param_1 + 0x54) * *(float *)(param_1 + 0x5c) * *(float *)(param_1 + 0x590);
    *(uint *)(param_1 + 0x49c) = *(uint *)(param_1 + 0x49c) & 0xfffffffb;
    pfVar1 = (float *)FUN_00437770(param_1);
    local_44 = *pfVar1;
    local_40 = pfVar1[1];
    local_3c = pfVar1[2];
    if ((double)local_44 != _DAT_0056cd50) {
      D3DXMatrixRotationX(local_c4,local_44);
      D3DXMatrixMultiply(param_1 + 0x57c,param_1 + 0x57c,local_c4);
    }
    if ((double)local_40 != _DAT_0056cd50) {
      D3DXMatrixRotationY(local_c4,local_40);
      D3DXMatrixMultiply(param_1 + 0x57c,param_1 + 0x57c,local_c4);
    }
    if ((double)local_3c != _DAT_0056cd50) {
      D3DXMatrixRotationZ(local_c4,local_3c);
      D3DXMatrixMultiply(param_1 + 0x57c,param_1 + 0x57c,local_c4);
    }
    *(uint *)(param_1 + 0x49c) = *(uint *)(param_1 + 0x49c) & 0xfffffffd;
  }
  puVar3 = (undefined4 *)(param_1 + 0x57c);
  p_Var4 = local_84;
  for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
    *(undefined4 *)p_Var4 = *puVar3;
    puVar3 = puVar3 + 1;
    p_Var4 = p_Var4 + 4;
  }
  local_54 = *(float *)(param_1 + 0x5bc) + *(float *)(param_1 + 0x2c) + *(float *)(param_1 + 0x484)
             + local_54;
  if (((*(byte *)(param_1 + 0x4a3) & 3) != 0) && (*(int *)(param_1 + 0x55c) == 0)) {
    local_54 = (float)DAT_005b87f8 / DAT_0056c8d0 + local_54;
    fStack_50 = ((float)DAT_005b87fc - DAT_0056d7c0) / DAT_0056c8d0 + fStack_50;
  }
  fStack_50 = *(float *)(param_1 + 0x5c0) + *(float *)(param_1 + 0x30) + *(float *)(param_1 + 0x488)
              + fStack_50;
  fStack_4c = *(float *)(param_1 + 0x34) + *(float *)(param_1 + 0x5c4) + *(float *)(param_1 + 0x48c)
  ;
  if ((*(int *)(param_1 + 0x55c) != 0) && ((*(uint *)(param_1 + 0x49c) >> 0xc & 1) == 0)) {
    local_54 = *(float *)(*(int *)(param_1 + 0x55c) + 0x5bc) +
               *(float *)(*(int *)(param_1 + 0x55c) + 0x2c) +
               *(float *)(*(int *)(param_1 + 0x55c) + 0x484) + local_54;
    fStack_50 = *(float *)(*(int *)(param_1 + 0x55c) + 0x5c0) +
                *(float *)(*(int *)(param_1 + 0x55c) + 0x30) +
                *(float *)(*(int *)(param_1 + 0x55c) + 0x488) + fStack_50;
    fStack_4c = *(float *)(*(int *)(param_1 + 0x55c) + 0x5c4) +
                *(float *)(*(int *)(param_1 + 0x55c) + 0x34) +
                *(float *)(*(int *)(param_1 + 0x55c) + 0x48c) + fStack_4c;
  }
  FUN_0040bc20(&local_38,0xc,4,FUN_00422e10);
  iVar2 = *(int *)(param_1 + 0x4a8);
  if (iVar2 == 0) {
    uStack_20 = DAT_0056cdbc;
    local_38 = DAT_0056cdbc;
    uStack_14 = DAT_0056cd98;
    uStack_2c = DAT_0056cd98;
  }
  else if ((iVar2 == 1) || (iVar2 != 2)) {
    uStack_20 = 0;
    local_38 = 0;
    uStack_14 = DAT_0056cda0;
    uStack_2c = DAT_0056cda0;
  }
  else {
    uStack_20 = DAT_0056e1a4;
    local_38 = DAT_0056e1a4;
    uStack_14 = 0;
    uStack_2c = 0;
  }
  iVar2 = *(int *)(param_1 + 0x4ac);
  if (iVar2 == 0) {
    uStack_28 = DAT_0056cdbc;
    uStack_34 = DAT_0056cdbc;
    uStack_10 = DAT_0056cd98;
    uStack_1c = DAT_0056cd98;
  }
  else if ((iVar2 == 1) || (iVar2 != 2)) {
    uStack_28 = 0;
    uStack_34 = 0;
    uStack_10 = DAT_0056cda0;
    uStack_1c = DAT_0056cda0;
  }
  else {
    uStack_28 = DAT_0056e1a4;
    uStack_34 = DAT_0056e1a4;
    uStack_10 = 0;
    uStack_1c = 0;
  }
  uStack_c = 0;
  uStack_18 = 0;
  uStack_24 = 0;
  uStack_30 = 0;
  p_Var4 = local_84;
  puVar3 = (undefined4 *)((int)this + 0x60007d8);
  for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = *(undefined4 *)p_Var4;
    p_Var4 = p_Var4 + 4;
    puVar3 = puVar3 + 1;
  }
  local_38 = uStack_20;
  uStack_34 = uStack_28;
  uStack_2c = uStack_14;
  uStack_1c = uStack_10;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

