/* Entry: 0x0052eb30; symbol: FUN_0052eb30; body bytes: 46 */

void * __thiscall
FUN_0052eb30(void *this,void *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 *puVar2;
  
  iVar1 = FUN_004fbff0((int)this);
  puVar2 = (undefined4 *)move<>(iVar1);
  _String_iterator<>(param_1,param_4,puVar2);
  return param_1;
}

