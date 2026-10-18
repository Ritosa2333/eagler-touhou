/* Entry: 0x0049ddc0; symbol: FUN_0049ddc0; body bytes: 194 */

void __fastcall FUN_0049ddc0(int *param_1)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  int local_18;
  undefined4 *local_14;
  int local_10;
  undefined4 *local_c;
  
  if (*param_1 != 0) {
    local_14 = (undefined4 *)param_1[5];
    for (local_10 = 0; local_10 < *param_1 + -1; local_10 = local_10 + 1) {
      local_c = (undefined4 *)FUN_0049dff0(*(int *)(param_1[4] + local_10 * 4));
      for (local_18 = 0; local_18 < param_1[1]; local_18 = local_18 + 1) {
        puVar2 = local_14;
        puVar3 = local_c;
        for (iVar1 = 7; iVar1 != 0; iVar1 = iVar1 + -1) {
          *puVar3 = *puVar2;
          puVar2 = puVar2 + 1;
          puVar3 = puVar3 + 1;
        }
        puVar2 = local_14 + param_1[1] * 7;
        puVar3 = local_c + 7;
        for (iVar1 = 7; iVar1 != 0; iVar1 = iVar1 + -1) {
          *puVar3 = *puVar2;
          puVar2 = puVar2 + 1;
          puVar3 = puVar3 + 1;
        }
        local_c = local_c + 0xe;
        local_14 = local_14 + 7;
      }
    }
  }
  return;
}

