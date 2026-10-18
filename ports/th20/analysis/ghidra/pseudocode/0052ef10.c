/* Entry: 0x0052ef10; symbol: FUN_0052ef10; body bytes: 81 */

void * __thiscall FUN_0052ef10(void *this,void *param_1)

{
  int iVar1;
  undefined4 uVar2;
  int *piVar3;
  int *piVar4;
  
  iVar1 = FUN_004fbff0((int)this);
  uVar2 = move<>(iVar1);
  piVar3 = (int *)FUN_0052eca0((int)this);
  piVar4 = (int *)Mysize((int)this);
  default_error_condition(param_1,*piVar3 + *piVar4,uVar2);
  return param_1;
}

