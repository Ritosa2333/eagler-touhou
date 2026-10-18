/* Entry: 0x0048b780; symbol: FUN_0048b780; body bytes: 130 */

void __fastcall FUN_0048b780(undefined4 *param_1)

{
  _Facet_base *p_Var1;
  undefined4 local_14;
  undefined4 *local_10;
  _Facet_base *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_10 = param_1;
  if (param_1[1] != 0) {
    local_14 = 0;
    FUN_0040cd70(param_1[1],&local_14);
    local_c = (_Facet_base *)local_10[2];
    while (local_c != (_Facet_base *)0x0) {
      p_Var1 = std::exchange<class_std::_Facet_base*,class_std::_Facet_base*>
                         (&local_c,(_Facet_base **)local_c);
      FUN_00489ac0((void *)*local_10,(char **)p_Var1);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

