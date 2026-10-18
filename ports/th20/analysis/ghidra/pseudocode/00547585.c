/* Entry: 0x00547585; symbol: FUN_00547585; body bytes: 153 */

undefined4 __cdecl FUN_00547585(int param_1,int *param_2)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  int *in_EAX;
  int iVar4;
  int *piVar5;
  undefined1 uVar6;
  int iVar7;
  int local_c;
  int local_8;
  
  if (param_2 == (int *)0x0) {
                    /* WARNING: Subroutine does not return */
    _abort();
  }
  iVar7 = *param_2;
  uVar6 = 0;
  if (0 < iVar7) {
    local_8 = 0;
    pbVar1 = *(byte **)(param_1 + 0x1c);
    piVar5 = *(int **)(pbVar1 + 0xc);
    iVar2 = *piVar5;
    in_EAX = piVar5 + 1;
    uVar6 = 0;
    do {
      if (0 < iVar2) {
        iVar3 = param_2[1];
        piVar5 = in_EAX;
        local_c = iVar2;
        do {
          iVar4 = FID_conflict____TypeMatch((byte *)(iVar3 + local_8),(byte *)*piVar5,pbVar1);
          if (iVar4 != 0) {
            uVar6 = 1;
            break;
          }
          local_c = local_c + -1;
          piVar5 = piVar5 + 1;
        } while (0 < local_c);
      }
      local_8 = local_8 + 0x10;
      iVar7 = iVar7 + -1;
    } while (iVar7 != 0);
  }
  return CONCAT31((int3)((uint)in_EAX >> 8),uVar6);
}

