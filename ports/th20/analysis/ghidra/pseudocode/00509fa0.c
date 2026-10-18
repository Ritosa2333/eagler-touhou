/* Entry: 0x00509fa0; symbol: FUN_00509fa0; body bytes: 85 */

void __thiscall FUN_00509fa0(void *this,int param_1)

{
  undefined4 *puVar1;
  void *pvVar2;
  undefined4 *local_8;
  
  local_8 = (undefined4 *)FUN_0040ff90((int)this + param_1 * 0x14 + 0x40);
  while (local_8 != (undefined4 *)0x0) {
    puVar1 = (undefined4 *)FUN_0040ff90((int)local_8);
    pvVar2 = (void *)FUN_0040c300(local_8);
    FUN_00506f10(pvVar2);
    local_8 = puVar1;
  }
  return;
}

