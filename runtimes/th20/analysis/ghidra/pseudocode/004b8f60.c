/* Entry: 0x004b8f60; symbol: FUN_004b8f60; body bytes: 230 */

void __fastcall FUN_004b8f60(int param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  int iVar2;
  undefined4 *puVar3;
  undefined4 local_14;
  undefined4 local_10;
  int local_c;
  int local_8;
  
  local_c = param_1;
  bVar1 = FUN_00485a10((undefined4 *)(param_1 + 0xcc));
  if (CONCAT31(extraout_var,bVar1) == 0) {
    local_8 = 0x96;
    iVar2 = FUN_00499330(0x5ba568);
    if (iVar2 < 0x29) {
      if (*(int *)(DAT_005c6110 + 200) < 0) {
        local_8 = -1;
      }
      else {
        local_8 = local_8 + *(int *)(DAT_005c6110 + 200);
      }
    }
    else if (*(int *)(DAT_005c6110 + 0x94) < 0) {
      local_8 = -1;
    }
    else {
      local_8 = local_8 + *(int *)(DAT_005c6110 + 0x94);
    }
    if (-1 < local_8) {
      local_10 = *(undefined4 *)(local_c + 0x2cc);
      puVar3 = FUN_00450c70(&local_14,(uint *)0x0,local_8,-1,(undefined4 *)0x0);
      *(undefined4 *)(local_c + 0xcc) = *puVar3;
    }
  }
  return;
}

