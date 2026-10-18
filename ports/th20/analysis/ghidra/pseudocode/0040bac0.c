/* Entry: 0x0040bac0; symbol: FUN_0040bac0; body bytes: 94 */

undefined4 * __thiscall FUN_0040bac0(void *this,int param_1)

{
  void *pvVar1;
  undefined8 *puVar2;
  undefined4 uVar3;
  
  pvVar1 = (void *)move<>(param_1);
  if (this != pvVar1) {
    FUN_0040bff0((undefined4 *)this);
    puVar2 = (undefined8 *)move<>(param_1);
    FUN_0040bb60(this,puVar2);
    uVar3 = move<>(param_1 + 8);
    FUN_0040bb20(uVar3);
  }
  return (undefined4 *)this;
}

