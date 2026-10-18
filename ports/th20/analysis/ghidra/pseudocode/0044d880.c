/* Entry: 0x0044d880; symbol: FUN_0044d880; body bytes: 234 */

void __fastcall FUN_0044d880(int param_1)

{
  undefined4 uVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  int iVar3;
  default_delete<class_std::_Facet_base> *local_14;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  uVar1 = FUN_0040c310(&DAT_005c0240,0x11);
  FUN_0040b730(&local_c,uVar1);
  if ((DAT_005c5888 >> 5 & 3) == 0) {
    local_14 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(param_1 + 0x6000730));
    pdVar2 = (default_delete<class_std::_Facet_base> *)FUN_0041be70(param_1 + 0x6000730);
    for (; local_14 != pdVar2; local_14 = local_14 + 4) {
      if (*(int *)local_14 != 0) {
        if (*(int *)(*(int *)local_14 + 0x60) != 0) {
          FUN_0040b900(&local_c);
          goto LAB_0044d95e;
        }
        iVar3 = FUN_004292c0(*(int *)local_14 + 0x5c);
        if (iVar3 != 0) {
          FUN_0040b900(&local_c);
          goto LAB_0044d95e;
        }
      }
    }
    FUN_0040b900(&local_c);
  }
  else {
    FUN_0040b900(&local_c);
  }
LAB_0044d95e:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

