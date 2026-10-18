/* Entry: 0x00469640; symbol: FUN_00469640; body bytes: 220 */

void __cdecl FUN_00469640(uint param_1,char *param_2)

{
  uint uVar1;
  uint local_c;
  byte local_5;
  
  if (param_1 == 0) {
    local_5 = 0x30;
    FUN_0046a230(param_2,9,&local_5);
  }
  else {
    for (local_c = 0; local_c < 5; local_c = local_c + 4) {
      uVar1 = param_1 % 10000;
      param_1 = param_1 / 10000;
      FUN_00543e20((uint *)(param_2 + (7 - local_c)),(uint *)(&DAT_0056f350 + (uVar1 % 100) * 2),2);
      FUN_00543e20((uint *)(param_2 + (5 - local_c)),(uint *)(&DAT_0056f350 + (uVar1 / 100) * 2),2);
    }
    *param_2 = (char)param_1 + '0';
  }
  return;
}

