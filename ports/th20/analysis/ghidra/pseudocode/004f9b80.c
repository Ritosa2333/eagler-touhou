/* Entry: 0x004f9b80; symbol: FUN_004f9b80; body bytes: 164 */

undefined4 FUN_004f9b80(int *param_1,undefined1 (*param_2) [16])

{
  int iVar1;
  undefined4 uVar2;
  undefined4 local_8;
  
  iVar1 = FUN_00410aa0(param_2,(SIZE_T *)0x0,0);
  *param_1 = iVar1;
  if (*param_1 == 0) {
    uVar2 = 0xffffffff;
  }
  else {
    for (local_8 = 0; local_8 < (int)(uint)*(ushort *)(*param_1 + 2); local_8 = local_8 + 1) {
      iVar1 = move<>(*(undefined4 *)(*param_1 + 0x5d4 + local_8 * 4));
      if (-1 < iVar1) {
        iVar1 = FUN_00471160(*(int *)(*param_1 + 0x5d4 + local_8 * 4),*param_1);
        *(int *)(*param_1 + 0x5d4 + local_8 * 4) = iVar1;
      }
    }
    uVar2 = 0;
  }
  return uVar2;
}

