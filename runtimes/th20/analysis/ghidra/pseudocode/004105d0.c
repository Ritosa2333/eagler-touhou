/* Entry: 0x004105d0; symbol: FUN_004105d0; body bytes: 53 */

void __thiscall FUN_004105d0(void *this,undefined4 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 local_14 [2];
  code *local_c;
  void *local_8;
  
                    /* WARNING: Load size is inaccurate */
  local_c = *(code **)(*this + 0xc);
  puVar2 = local_14;
  local_8 = this;
  puVar1 = (undefined4 *)(*local_c)(puVar2,param_1,param_2);
  FUN_0040e760(puVar1,puVar2);
  return;
}

