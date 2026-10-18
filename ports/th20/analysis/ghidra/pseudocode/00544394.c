/* Entry: 0x00544394; symbol: FUN_00544394; body bytes: 110 */

void __cdecl
FUN_00544394(undefined4 *param_1,undefined4 param_2,int param_3,undefined4 param_4,int param_5,
            int param_6)

{
  int *piVar1;
  uint uVar2;
  uint uVar3;
  uint local_8;
  
  uVar2 = *(uint *)(param_5 + 0xc);
  uVar3 = uVar2;
  if (-1 < param_6) {
    piVar1 = (int *)(uVar2 * 0x14 + *(int *)(param_5 + 0x10) + 8);
    local_8 = uVar2;
    do {
      if (uVar2 == 0xffffffff) goto LAB_005443fd;
      uVar2 = uVar2 - 1;
      if (((piVar1[-6] < param_3) && (param_3 <= piVar1[-5])) || (uVar2 == 0xffffffff)) {
        param_6 = param_6 + -1;
        uVar3 = local_8;
        local_8 = uVar2;
      }
      piVar1 = piVar1 + -5;
    } while (-1 < param_6);
  }
  if ((uVar3 <= *(uint *)(param_5 + 0xc)) && (uVar2 + 1 <= uVar3)) {
    param_1[3] = uVar3;
    *param_1 = param_2;
    param_1[1] = uVar2 + 1;
    param_1[2] = param_2;
    return;
  }
LAB_005443fd:
                    /* WARNING: Subroutine does not return */
  _abort();
}

