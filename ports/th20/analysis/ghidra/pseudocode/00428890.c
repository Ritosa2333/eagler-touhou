/* Entry: 0x00428890; symbol: FUN_00428890; body bytes: 255 */

void __thiscall FUN_00428890(void *this,int param_1)

{
  int *piVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  int iVar3;
  default_delete<class_std::_Facet_base> *local_10;
  int local_c;
  int local_8;
  
  if (param_1 < 0) {
    for (local_c = 0; local_c < 0x5a; local_c = local_c + 1) {
      piVar1 = (int *)FUN_004260f0((void *)((int)this + 0x1994),local_c);
      FUN_00428810(piVar1);
    }
  }
  else {
    local_8 = 0;
    local_10 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)((int)this + 0x1c));
    pdVar2 = (default_delete<class_std::_Facet_base> *)FUN_00428c40((int)this + 0x1c);
    for (; (local_10 != pdVar2 && (-1 < *(int *)local_10)); local_10 = local_10 + 0x208) {
      if (*(int *)local_10 == param_1) {
        *(int *)(local_10 + 4) = -1;
        return;
      }
      local_8 = local_8 + 1;
    }
    if (local_8 < 0xc) {
      piVar1 = (int *)FUN_004260b0((void *)((int)this + 0x1c),local_8);
      *piVar1 = param_1;
      iVar3 = FUN_004260b0((void *)((int)this + 0x1c),local_8);
      *(undefined4 *)(iVar3 + 4) = 0xffffffff;
    }
  }
  return;
}

