/* Entry: 0x004c5160; symbol: FUN_004c5160; body bytes: 343 */

void __thiscall FUN_004c5160(void *this,undefined4 param_1)

{
  int iVar1;
  int iVar2;
  default_delete<class_std::_Facet_base> *pdVar3;
  default_delete<class_std::_Facet_base> *pdVar4;
  int iVar5;
  allocator<struct_std::_Container_proxy> *this_00;
  _Container_proxy *p_Var6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  
  pdVar3 = FUN_004b7880((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  pdVar4 = FUN_004b78a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  if ((uint)(*(int *)pdVar3 + *(int *)pdVar4) % 4 == 0) {
    pdVar3 = FUN_004b6de0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)this);
    pdVar4 = FUN_004b78a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)this);
    if (*(uint *)pdVar3 <= *(int *)pdVar4 + 4U >> 2) {
      FUN_004c59c0(this,1);
    }
  }
  pdVar3 = FUN_004b6de0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  iVar1 = *(int *)pdVar3;
  pdVar3 = FUN_004b7880((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  *(uint *)pdVar3 = *(uint *)pdVar3 & iVar1 * 4 - 1U;
  pdVar3 = FUN_004b7880((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  pdVar4 = FUN_004b78a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  iVar1 = *(int *)pdVar3;
  iVar2 = *(int *)pdVar4;
  iVar5 = FUN_004c5990(this,iVar1 + iVar2);
  pdVar3 = FUN_004b6da0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  if (*(int *)(*(int *)pdVar3 + iVar5 * 4) == 0) {
    this_00 = (allocator<struct_std::_Container_proxy> *)
              FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                            *)this);
    p_Var6 = std::allocator<struct_std::_Container_proxy>::allocate(this_00,4);
    pdVar3 = FUN_004b6da0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)this);
    *(_Container_proxy **)(*(int *)pdVar3 + iVar5 * 4) = p_Var6;
  }
  uVar7 = move<>(param_1);
  pdVar3 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  uVar8 = FUN_004b6580(pdVar3,iVar1 + iVar2);
  uVar9 = FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  FUN_004c53f0(uVar9,uVar8,uVar7);
  pdVar3 = FUN_004b78a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)this);
  *(int *)pdVar3 = *(int *)pdVar3 + 1;
  return;
}

