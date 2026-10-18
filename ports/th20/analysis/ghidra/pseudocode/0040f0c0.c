/* Entry: 0x0040f0c0; symbol: FUN_0040f0c0; body bytes: 57 */

void __thiscall FUN_0040f0c0(void *this,int param_1)

{
  undefined4 *puVar1;
  undefined2 *puVar2;
  undefined2 local_6;
  
  *(int *)((int)this + 0x10) = param_1;
  local_6 = 0;
  puVar2 = &local_6;
  puVar1 = Myptr((undefined4 *)this);
  FUN_0040ff00((undefined2 *)((int)puVar1 + param_1 * 2),puVar2);
  return;
}

