/* Entry: 0x00541c22; symbol: FUN_00541c22; body bytes: 76 */

void __cdecl FUN_00541c22(int param_1,uint *param_2)

{
  uint *puVar1;
  uint *puVar2;
  
  puVar1 = (uint *)_setlocale(0,(char *)0x0);
  puVar2 = (uint *)&DAT_00570afc;
  if (puVar1 != (uint *)0x0) {
    puVar2 = puVar1;
  }
  FUN_00541a86((void *)(param_1 + 0x24),puVar2);
  if (param_2 != (uint *)0x0) {
    param_2 = (uint *)_setlocale(0,(char *)param_2);
  }
  puVar2 = (uint *)&DAT_00596018;
  if (param_2 != (uint *)0x0) {
    puVar2 = param_2;
  }
  FUN_00541a86((void *)(param_1 + 0x2c),puVar2);
  return;
}

