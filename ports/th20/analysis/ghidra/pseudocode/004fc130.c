/* Entry: 0x004fc130; symbol: FUN_004fc130; body bytes: 46 */

uint * FUN_004fc130(uint *param_1)

{
  _Cvtvec *p_Var1;
  int iVar2;
  uint *puVar3;
  _Cvtvec local_34;
  
  p_Var1 = __Getcvt(&local_34);
  puVar3 = param_1;
  for (iVar2 = 0xb; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = p_Var1->_Page;
    p_Var1 = (_Cvtvec *)&p_Var1->_Mbcurmax;
    puVar3 = puVar3 + 1;
  }
  return param_1;
}

