/* Entry: 0x0040c280; symbol: FUN_0040c280; body bytes: 70 */

uint __thiscall FUN_0040c280(void *this,uint param_1,char **param_2)

{
  uint *puVar1;
  uint uVar2;
  uint uVar3;
  char **unaff_ESI;
  bool bVar4;
  
  std::_Adl_verify_range<char*,char_const*>(param_2,unaff_ESI);
  puVar1 = (uint *)move<>(this);
  uVar2 = *puVar1;
  do {
    LOCK();
    uVar3 = *puVar1;
    bVar4 = uVar2 == uVar3;
    if (bVar4) {
      *puVar1 = uVar2 | param_1;
      uVar3 = uVar2;
    }
    UNLOCK();
    uVar2 = uVar3;
  } while (!bVar4);
  return uVar3;
}

