/* Entry: 0x0052efe0; symbol: FUN_0052efe0; body bytes: 67 */

void * __thiscall FUN_0052efe0(void *this,void *param_1)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  iVar1 = FUN_004fbff0((int)this);
  puVar2 = (undefined4 *)move<>(iVar1);
  puVar3 = (undefined4 *)FUN_0052eca0((int)this);
  _String_iterator<>(param_1,*puVar3,puVar2);
  return param_1;
}

