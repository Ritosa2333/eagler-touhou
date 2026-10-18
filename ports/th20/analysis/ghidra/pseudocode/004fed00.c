/* Entry: 0x004fed00; symbol: FUN_004fed00; body bytes: 75 */

void * __thiscall FUN_004fed00(void *this,void *param_1)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  
  move<>((int)this + 4);
  puVar1 = Myptr((undefined4 *)((int)this + 4));
  uVar2 = move<>(puVar1);
  _Vector_iterator<>(param_1,uVar2);
  return param_1;
}

