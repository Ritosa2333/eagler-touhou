/* Entry: 0x00428560; symbol: FUN_00428560; body bytes: 325 */

undefined4 __fastcall FUN_00428560(int *param_1)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  undefined4 uVar2;
  float fVar3;
  default_delete<class_std::_Facet_base> *local_10;
  
  local_10 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(param_1 + 7));
  pdVar1 = (default_delete<class_std::_Facet_base> *)FUN_00428c40((int)(param_1 + 7));
  for (; local_10 != pdVar1; local_10 = local_10 + 0x208) {
    *(undefined4 *)local_10 = 0xffffffff;
  }
  if (param_1[3] == 0) {
    uVar2 = 0xffffffff;
  }
  else if (*param_1 == 0) {
    uVar2 = 0;
  }
  else {
    param_1[0x15f7] = (int)DAT_005c4f86;
    param_1[0x15f8] = (int)DAT_005c4f87;
    if (param_1[0x15f8] == 0) {
      param_1[0x15f9] = -10000;
    }
    else {
      fVar3 = DAT_0056c8cc - (float)param_1[0x15f7] / DAT_0056e04c;
      fVar3 = fVar3 * fVar3;
      param_1[0x15f9] = (int)(DAT_0056cdb8 * (DAT_0056c8cc - fVar3 * fVar3)) + -5000;
    }
    param_1[0x15f9] = (param_1[0x15f9] * 9) / 10;
    uVar2 = 0;
  }
  return uVar2;
}

