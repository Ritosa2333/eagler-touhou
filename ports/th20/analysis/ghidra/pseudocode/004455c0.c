/* Entry: 0x004455c0; symbol: FUN_004455c0; body bytes: 192 */

void __fastcall FUN_004455c0(int param_1)

{
  int *piVar1;
  
  if (*(int *)(param_1 + 0x6000e70) != 0) {
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x164))(piVar1,0x144);
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x14c))
              (piVar1,4,*(int *)(param_1 + 0x6000e70) << 1,*(undefined4 *)(param_1 + 0x7c00e78),0x1c
              );
    *(undefined4 *)(param_1 + 0x7c00e78) = *(undefined4 *)(param_1 + 0x7c00e74);
    *(undefined4 *)(param_1 + 0x6000e70) = 0;
    *(int *)(param_1 + 0xc4) = *(int *)(param_1 + 0xc4) + 1;
  }
  return;
}

