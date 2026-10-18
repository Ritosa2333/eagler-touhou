/* Entry: 0x0051c150; symbol: FUN_0051c150; body bytes: 130 */

void __thiscall FUN_0051c150(void *this,undefined1 param_1)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined1 local_5;
  
  uVar1 = *(uint *)((int)this + 0x14);
  if (uVar1 < *(uint *)((int)this + 0x18)) {
    *(uint *)((int)this + 0x14) = uVar1 + 1;
    puVar2 = Myptr((undefined4 *)((int)this + 4));
    FUN_0040fe90((undefined1 *)((int)puVar2 + uVar1),&param_1);
    local_5 = 0;
    FUN_0040fe90((undefined1 *)((int)puVar2 + uVar1 + 1),&local_5);
  }
  else {
    FUN_00514600(this,1);
  }
  return;
}

