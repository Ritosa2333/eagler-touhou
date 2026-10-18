/* Entry: 0x004a1630; symbol: FUN_004a1630; body bytes: 127 */

void __thiscall FUN_004a1630(void *this,undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  
                    /* WARNING: Load size is inaccurate */
  uVar1 = move<>(0x20,*this);
  move<>(param_1);
  puVar2 = (undefined4 *)move<>(param_3);
  FUN_004a2e10(&stack0xffffffe4,puVar2);
  puVar2 = (undefined4 *)move<>(param_2);
  FUN_004a2de0(&stack0xffffffe0,puVar2);
  FUN_004a1090(uVar1);
  return;
}

