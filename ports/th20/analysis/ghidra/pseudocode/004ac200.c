/* Entry: 0x004ac200; symbol: FUN_004ac200; body bytes: 42 */

undefined8 __cdecl FUN_004ac200(COleCurrency *param_1)

{
  undefined8 *puVar1;
  tagCY tVar2;
  undefined8 uVar3;
  undefined1 local_14 [16];
  
  tVar2 = COleCurrency::operator_union_tagCY(param_1);
  puVar1 = (undefined8 *)FUN_004abb20(local_14,(undefined8 *)tVar2.s.Lo);
  uVar3 = FUN_0044c090(puVar1);
  return uVar3;
}

