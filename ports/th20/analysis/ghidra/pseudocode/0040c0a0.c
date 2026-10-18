/* Entry: 0x0040c0a0; symbol: FUN_0040c0a0; body bytes: 147 */

void __thiscall FUN_0040c0a0(void *this,undefined4 *param_1,undefined4 param_2,char **param_3)

{
  uint uVar1;
  uint uVar2;
  uint *puVar3;
  uint *puVar4;
  char **unaff_EDI;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  uVar1 = FUN_0040b2a0(param_1);
  std::_Adl_verify_range<char*,char_const*>(param_3,unaff_EDI);
  uVar2 = FUN_0040b2a0(&param_2);
  puVar3 = (uint *)move<>(this);
  LOCK();
  local_c = *puVar3;
  if (uVar1 == local_c) {
    *puVar3 = uVar2;
    local_c = uVar1;
  }
  UNLOCK();
  if (local_c != uVar1) {
    uVar1 = 4;
    puVar3 = &local_c;
    puVar4 = (uint *)move<>(param_1);
    FUN_00543e20(puVar4,puVar3,uVar1);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

