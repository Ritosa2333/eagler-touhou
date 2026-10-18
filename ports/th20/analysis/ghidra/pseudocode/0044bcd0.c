/* Entry: 0x0044bcd0; symbol: FUN_0044bcd0; body bytes: 118 */

void FUN_0044bcd0(int param_1)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  undefined4 local_14;
  undefined4 *local_10;
  undefined4 *local_c;
  int local_8;
  
  local_8 = FUN_0044cd00();
  if (local_8 != 0) {
    local_c = (undefined4 *)FUN_00437470(local_8);
    FUN_0044fd00(local_8);
    puVar1 = FUN_00450c70(&local_14,(uint *)0x0,param_1,-1,(undefined4 *)0x0);
    *local_10 = *puVar1;
    uVar2 = FUN_0040c300(local_c);
    *(undefined4 *)(local_8 + 0x18) = uVar2;
    uVar2 = FUN_0040c300(local_c);
    *(undefined4 *)(local_8 + 0x1c) = uVar2;
  }
  return;
}

