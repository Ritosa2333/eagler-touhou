/* Entry: 0x0053e920; symbol: FUN_0053e920; body bytes: 72 */

undefined4 * __thiscall FUN_0053e920(void *this,int param_1)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 *local_8;
  
  puVar1 = (undefined4 *)((int)this + 0x5c);
  do {
    local_8 = puVar1;
    if (local_8 == (undefined4 *)0x0) {
      return (undefined4 *)0x0;
    }
    puVar1 = (undefined4 *)FUN_0040ff90((int)local_8);
    iVar2 = FUN_0040c300(local_8);
  } while (*(int *)(iVar2 + 0x24) != param_1);
  return local_8;
}

