/* Entry: 0x004dd1c0; symbol: FUN_004dd1c0; body bytes: 275 */

void FUN_004dd1c0(void)

{
  int *piVar1;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (DAT_005c4edc != 0) {
    FUN_004455c0(DAT_005c0028);
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x94))(piVar1,0,DAT_005c4edc);
    local_18 = DAT_005c54dc;
    local_14 = DAT_005c54e0;
    local_10 = DAT_005c54dc + DAT_005c54e4;
    local_c = DAT_005c54e0 + DAT_005c54e8;
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0xac))(piVar1,1,&local_18,2,DAT_005c5b20,DAT_0056c8cc,0);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

