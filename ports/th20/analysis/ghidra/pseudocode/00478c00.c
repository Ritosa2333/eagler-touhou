/* Entry: 0x00478c00; symbol: FUN_00478c00; body bytes: 500 */

void FUN_00478c00(void)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 local_40;
  undefined4 local_3c;
  void *local_38;
  void *local_34;
  void *local_30;
  void *local_2c;
  undefined4 local_28;
  void *local_24;
  void *local_20;
  undefined4 local_1c;
  int local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422e10(&local_14);
  iVar1 = FUN_00460830(0);
  puVar2 = (undefined4 *)FUN_00460850(iVar1);
  *(undefined4 *)(local_18 + 0xb8) = *puVar2;
  *(undefined4 *)(local_18 + 0xbc) = puVar2[1];
  *(undefined4 *)(local_18 + 0xc0) = puVar2[2];
  FUN_00452f20((void *)(local_18 + 0xc4),DAT_0056e0f8 / DAT_0056c8d0);
  FUN_00426d70(&DAT_005ba830,0x31,0);
  iVar1 = FUN_00460830(0);
  local_1c = FUN_00421810(iVar1);
  puVar2 = FUN_004790e0(&local_3c,(uint *)&DAT_0056fb24,0x33,(float *)(local_18 + 0xb8),0,-1,
                        (undefined4 *)0x0);
  *(undefined4 *)(local_18 + 200) = *puVar2;
  local_20 = (void *)(local_18 + 200);
  FUN_00450850(local_20,local_18);
  local_24 = (void *)(local_18 + 200);
  FUN_0044f100(local_24,FUN_00478380);
  iVar1 = FUN_00460830(0);
  local_28 = FUN_00421810(iVar1);
  puVar2 = FUN_004790e0(&local_40,(uint *)&DAT_0056fb24,0x41,(float *)(local_18 + 0xb8),0,-1,
                        (undefined4 *)0x0);
  *(undefined4 *)(local_18 + 0x74) = *puVar2;
  local_2c = (void *)(local_18 + 0x74);
  FUN_00450850(local_2c,local_18);
  local_30 = (void *)(local_18 + 0x74);
  FUN_0044f100(local_30,FUN_00478380);
  local_14 = *(undefined4 *)(local_18 + 0xb8);
  local_10 = *(undefined4 *)(local_18 + 0xbc);
  local_c = *(undefined4 *)(local_18 + 0xc0);
  iVar1 = FUN_00478ea0(0);
  FUN_00487bb0(iVar1);
  local_34 = (void *)FUN_00460830(0);
  FUN_00479060(local_34,0x78);
  local_38 = (void *)FUN_00478060(0);
  FUN_00477f40(local_38,1);
  __aligned_offset_recalloc((void *)0x8,3,0x3c,0xf0,0x1e);
  iVar1 = FUN_00460830(0);
  FUN_004790a0(iVar1);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

