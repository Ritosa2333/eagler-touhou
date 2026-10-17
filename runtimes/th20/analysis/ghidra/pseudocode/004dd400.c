/* Entry: 0x004dd400; symbol: FUN_004dd400; body bytes: 135 */

undefined4 FUN_004dd400(void)

{
  int *piVar1;
  
  FUN_004455c0(DAT_005c0028);
  piVar1 = (int *)FUN_00412730(0x5c4d40);
  (**(code **)(*piVar1 + 0x94))(piVar1,0,DAT_005c4ee4);
  piVar1 = (int *)FUN_00412730(0x5c4d40);
  (**(code **)(*piVar1 + 0xac))(piVar1,0,0,3,0xff000000,DAT_0056c8cc,0);
  return 1;
}

