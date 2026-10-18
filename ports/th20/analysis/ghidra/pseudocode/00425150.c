/* Entry: 0x00425150; symbol: FUN_00425150; body bytes: 56 */

void __fastcall FUN_00425150(undefined4 *param_1)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)move<>(0x10,*param_1);
  *puVar1 = 0;
  puVar1[1] = 0;
  puVar1[2] = 0;
  puVar1[3] = 0;
  FUN_0040de30(puVar1);
  return;
}

