/* Entry: 0x0040bb60; symbol: FUN_0040bb60; body bytes: 83 */

undefined4 * __thiscall FUN_0040bb60(void *this,undefined8 *param_1)

{
  bool bVar1;
  undefined8 uVar2;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 *local_8;
  
  local_8 = (undefined4 *)this;
  bVar1 = FUN_0040c3f0((int)this);
  if (bVar1) {
    terminate();
  }
  local_18 = 0;
  local_14 = 0;
  uVar2 = FUN_0040b4f0(param_1,&local_18);
  local_10 = (undefined4)uVar2;
  local_c = (undefined4)((ulonglong)uVar2 >> 0x20);
  *local_8 = local_10;
  local_8[1] = local_c;
  return local_8;
}

