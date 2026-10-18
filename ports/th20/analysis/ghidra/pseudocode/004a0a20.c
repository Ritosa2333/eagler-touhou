/* Entry: 0x004a0a20; symbol: FUN_004a0a20; body bytes: 79 */

undefined4 __thiscall FUN_004a0a20(void *this,int param_1)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 local_c;
  
  uVar1 = FUN_004495a0(this,1 << ((byte)param_1 & 0x1f));
  if (uVar1 == 0) {
    local_c = 0;
  }
  else {
    puVar2 = (undefined4 *)FUN_00414580((void *)((int)this + 0x198),param_1);
    local_c = *puVar2;
  }
  return local_c;
}

