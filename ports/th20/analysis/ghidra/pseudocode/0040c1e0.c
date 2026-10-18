/* Entry: 0x0040c1e0; symbol: FUN_0040c1e0; body bytes: 83 */

void __thiscall FUN_0040c1e0(void *this,undefined4 param_1,char **param_2)

{
  uint uVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  char **unaff_ESI;
  
  uVar1 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  std::_Adl_verify_range<char*,char_const*>(param_2,unaff_ESI);
  uVar2 = FUN_0040b2a0(&param_1);
  puVar3 = (undefined4 *)move<>(this);
  LOCK();
  *puVar3 = uVar2;
  UNLOCK();
  FUN_005429ee(uVar1 ^ (uint)&stack0xfffffffc);
  return;
}

