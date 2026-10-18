/* Entry: 0x00446e00; symbol: FUN_00446e00; body bytes: 63 */

void * __cdecl FUN_00446e00(void *param_1,COleCurrency *param_2,COleCurrency *param_3)

{
  tagCY tVar1;
  tagCY tVar2;
  
  tVar1 = COleCurrency::operator_union_tagCY(param_3);
  tVar2 = COleCurrency::operator_union_tagCY(param_2);
  FUN_00446e40(param_1,(undefined4 *)tVar2.s.Lo,(undefined4 *)tVar1.s.Lo);
  return param_1;
}

