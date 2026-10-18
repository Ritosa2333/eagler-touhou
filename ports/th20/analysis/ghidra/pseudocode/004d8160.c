/* Entry: 0x004d8160; symbol: FUN_004d8160; body bytes: 190 */

undefined4 __fastcall FUN_004d8160(int param_1)

{
  undefined4 *puVar1;
  undefined4 local_10;
  undefined4 local_c;
  int local_8;
  
  local_8 = param_1;
  if (*(int *)(param_1 + 0x610) == 1) {
    local_c = *(undefined4 *)(param_1 + 0x60c);
    puVar1 = FUN_00450c70(&local_10,(uint *)&DAT_0057190c,0,-1,(undefined4 *)0x0);
    *(undefined4 *)(local_8 + 0x608) = *puVar1;
    *(int *)(local_8 + 0x610) = *(int *)(local_8 + 0x610) + 1;
  }
  if (*(int *)(local_8 + 0x614) == 1) {
    FUN_004a0aa0(DAT_0056cda8,DAT_00570388);
    *(int *)(local_8 + 0x614) = *(int *)(local_8 + 0x614) + 1;
  }
  *(int *)(local_8 + 0x618) = *(int *)(local_8 + 0x618) + 1;
  return 1;
}

