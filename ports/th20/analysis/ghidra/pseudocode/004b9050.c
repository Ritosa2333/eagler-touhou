/* Entry: 0x004b9050; symbol: FUN_004b9050; body bytes: 58 */

void __fastcall FUN_004b9050(int param_1)

{
  undefined4 *puVar1;
  undefined4 local_10;
  undefined4 local_c;
  int local_8;
  
  local_8 = param_1;
  local_c = FUN_004b8250(param_1);
  puVar1 = FUN_00450c70(&local_10,(uint *)"front",0x30,-1,(undefined4 *)0x0);
  *(undefined4 *)(local_8 + 0x104) = *puVar1;
  return;
}

