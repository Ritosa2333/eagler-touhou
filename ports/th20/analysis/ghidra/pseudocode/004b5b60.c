/* Entry: 0x004b5b60; symbol: FUN_004b5b60; body bytes: 79 */

undefined4 __thiscall FUN_004b5b60(void *this,int param_1)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 local_c;
  
  uVar1 = FUN_004973a0(this,1 << ((byte)param_1 & 0x1f));
  if (uVar1 == 0) {
    local_c = 0;
  }
  else {
    puVar2 = (undefined4 *)FUN_00414580((void *)((int)this + 0x218),param_1);
    local_c = *puVar2;
  }
  return local_c;
}

