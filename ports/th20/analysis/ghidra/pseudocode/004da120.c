/* Entry: 0x004da120; symbol: FUN_004da120; body bytes: 194 */

void FUN_004da120(int param_1)

{
  int *piVar1;
  
  if (DAT_005c0028 != 0) {
    FUN_004455c0(DAT_005c0028);
  }
  piVar1 = (int *)FUN_00412730(0x5c4d40);
  (**(code **)(*piVar1 + 0xb0))(piVar1,2,param_1 + 0x60);
  piVar1 = (int *)FUN_00412730(0x5c4d40);
  (**(code **)(*piVar1 + 0xb0))(piVar1,3,param_1 + 0xa0);
  if (DAT_005c0028 != 0) {
    *(undefined4 *)(DAT_005c0028 + 200) = *(undefined4 *)(param_1 + 300);
    *(undefined4 *)(DAT_005c0028 + 0xcc) = *(undefined4 *)(param_1 + 0x130);
  }
  return;
}

