/* Entry: 0x00466e20; symbol: FUN_00466e20; body bytes: 343 */

void __thiscall FUN_00466e20(void *this,float param_1,float param_2,float param_3)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  undefined4 uVar2;
  int iVar3;
  char cVar4;
  float fVar5;
  default_delete<class_std::_Facet_base> *local_18;
  default_delete<class_std::_Facet_base> *local_10;
  int local_c;
  
  fVar5 = param_3 / DAT_0056f280;
  local_c = 0;
  cVar4 = '\0';
  local_10 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)((int)this + 0xf0));
  pdVar1 = (default_delete<class_std::_Facet_base> *)FUN_00466fb0((int)this + 0xf0);
  for (; local_10 != pdVar1; local_10 = local_10 + 4) {
    uVar2 = FUN_0040c300((undefined4 *)&stack0x00000010);
    *(undefined4 *)local_10 = uVar2;
    if (local_c < 8) {
      iVar3 = FUN_00414580((void *)((int)this + 0xf0),local_c);
      *(char *)(iVar3 + 3) = cVar4;
      cVar4 = cVar4 + ' ';
    }
    else if (0xd < local_c) {
      cVar4 = cVar4 + -8;
      iVar3 = FUN_00414580((void *)((int)this + 0xf0),local_c);
      *(char *)(iVar3 + 3) = cVar4;
    }
    local_c = local_c + 1;
  }
  local_18 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)this);
  pdVar1 = (default_delete<class_std::_Facet_base> *)FUN_0045dca0((int)this);
  for (; local_18 != pdVar1; local_18 = local_18 + 8) {
    FUN_00439330(local_18,(float *)local_18,param_1,param_3);
    param_1 = param_1 + param_2;
    param_3 = param_3 - fVar5;
  }
  return;
}

