/* Entry: 0x004a7da0; symbol: FUN_004a7da0; body bytes: 844 */

void __fastcall FUN_004a7da0(int param_1)

{
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var1;
  default_delete<class_std::_Facet_base> *pdVar2;
  void *this;
  float10 fVar3;
  int local_24;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_20;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_1c;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422e10(&local_14);
  local_1c = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_00414f30(param_1 + 0x158);
  p_Var1 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *)FUN_00414f70(param_1 + 0x158);
  for (; local_1c != p_Var1; local_1c = local_1c + 0x184) {
    pdVar2 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(local_1c);
    FUN_004296e0(&local_14,(float *)pdVar2);
  }
  pdVar2 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)(param_1 + 0x110));
  FUN_00429740(&local_14,(float *)pdVar2);
  FUN_0047a5e0((void *)(param_1 + 0x110),&local_14);
  FUN_00453ac0((float *)(param_1 + 0x110));
  if ((*(uint *)(param_1 + 0x2cc) >> 1 & 1) != 0) {
    fVar3 = FUN_004292e0((float *)(param_1 + 0x110));
    if (*(float *)(param_1 + 0x178) - *(float *)(param_1 + 0x180) / DAT_0056c8d0 <= (float)fVar3) {
      fVar3 = FUN_004292e0((float *)(param_1 + 0x110));
      if (*(float *)(param_1 + 0x180) / DAT_0056c8d0 + *(float *)(param_1 + 0x178) < (float)fVar3) {
        FUN_004292a0((void *)(param_1 + 0x110),
                     *(float *)(param_1 + 0x180) / DAT_0056c8d0 + *(float *)(param_1 + 0x178));
      }
    }
    else {
      FUN_004292a0((void *)(param_1 + 0x110),
                   *(float *)(param_1 + 0x178) - *(float *)(param_1 + 0x180) / DAT_0056c8d0);
    }
    fVar3 = FUN_00499390(param_1 + 0x110);
    if (*(float *)(param_1 + 0x17c) - *(float *)(param_1 + 0x184) / DAT_0056c8d0 <= (float)fVar3) {
      fVar3 = FUN_00499390(param_1 + 0x110);
      if (*(float *)(param_1 + 0x184) / DAT_0056c8d0 + *(float *)(param_1 + 0x17c) < (float)fVar3) {
        FUN_0049c7e0((void *)(param_1 + 0x110),
                     *(float *)(param_1 + 0x184) / DAT_0056c8d0 + *(float *)(param_1 + 0x17c));
      }
    }
    else {
      FUN_0049c7e0((void *)(param_1 + 0x110),
                   *(float *)(param_1 + 0x17c) - *(float *)(param_1 + 0x184) / DAT_0056c8d0);
    }
    pdVar2 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(param_1 + 0x110));
    local_14 = *(undefined4 *)pdVar2;
    local_10 = *(undefined4 *)(pdVar2 + 4);
    local_c = *(undefined4 *)(pdVar2 + 8);
    local_24 = 0;
    local_20 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)FUN_00414f30(param_1 + 0x158);
    p_Var1 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_00414f70(param_1 + 0x158);
    for (; local_20 != p_Var1; local_20 = local_20 + 0x184) {
      if (0 < local_24) {
        pdVar2 = std::
                 _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                 ::_Get_first(local_20);
        FUN_00429740(&local_14,(float *)pdVar2);
      }
      local_24 = local_24 + 1;
    }
    this = (void *)FUN_0048bd10((void *)(param_1 + 0x158),0);
    FUN_004393f0(this,&local_14);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

