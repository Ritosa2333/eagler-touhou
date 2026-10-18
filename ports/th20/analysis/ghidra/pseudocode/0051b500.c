/* Entry: 0x0051b500; symbol: FUN_0051b500; body bytes: 81 */

void * __thiscall FUN_0051b500(void *this,void *param_1)

{
  undefined4 *puVar1;
  int iVar2;
  
  move<>((int)this + 4);
  puVar1 = Myptr((undefined4 *)((int)this + 4));
  iVar2 = move<>(puVar1);
  _Vector_iterator<>(param_1,iVar2 + *(int *)((int)this + 0x14));
  return param_1;
}

