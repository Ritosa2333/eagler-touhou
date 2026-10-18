/* Entry: 0x0052eec0; symbol: FUN_0052eec0; body bytes: 67 */

void * __thiscall FUN_0052eec0(void *this,void *param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  
  iVar1 = FUN_004fbff0((int)this);
  uVar2 = move<>(iVar1);
  puVar3 = (undefined4 *)FUN_0052eca0((int)this);
  default_error_condition(param_1,*puVar3,uVar2);
  return param_1;
}

