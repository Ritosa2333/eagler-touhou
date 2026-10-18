/* Entry: 0x004b6220; symbol: FUN_004b6220; body bytes: 568 */

void __fastcall FUN_004b6220(void *param_1)

{
  bool bVar1;
  uint uVar2;
  undefined3 extraout_var;
  undefined4 *puVar3;
  void *this;
  undefined4 uVar4;
  undefined4 local_10;
  undefined4 local_c;
  
  uVar2 = FUN_00474e00(0x5ba568);
  if (uVar2 == 0) {
    uVar2 = FUN_004b8370(0x5ba568);
    if (uVar2 != 0) goto LAB_004b6255;
    FUN_0044c430(DAT_005c0028,6);
  }
  else {
LAB_004b6255:
    FUN_004863f0(DAT_005c0028,6,'\0');
  }
  *(undefined4 *)((int)param_1 + 0x170) = 0;
  bVar1 = FUN_00478160((int)param_1);
  if (CONCAT31(extraout_var,bVar1) != 0) {
    FUN_004ae830(*(void **)((int)param_1 + 0x1bc));
    *(undefined4 *)((int)param_1 + 0x1bc) = 0;
    *(undefined4 *)((int)param_1 + 0x1bc) = 0;
  }
  uVar2 = FUN_00474e00(0x5ba568);
  if (uVar2 == 0) {
    uVar2 = FUN_004b8370(0x5ba568);
    if (uVar2 == 0) {
      if (*(int *)((int)param_1 + 0x1c0) != 0) {
        FUN_0041f670(*(LPVOID *)((int)param_1 + 0x1c0));
        *(undefined4 *)((int)param_1 + 0x1c0) = 0;
      }
      *(undefined4 *)((int)param_1 + 0x1c0) = 0;
      DAT_005c4a14 = 0;
      goto LAB_004b631f;
    }
  }
  DAT_005c4a14 = *(undefined4 *)((int)param_1 + 0x1c0);
LAB_004b631f:
  if (*(int *)((int)param_1 + 8) != 0) {
    FUN_004127f0(*(int *)((int)param_1 + 8));
  }
  puVar3 = (undefined4 *)FUN_00414580((void *)((int)param_1 + 0xbc),0);
  FUN_0044fcd0(puVar3);
  puVar3 = (undefined4 *)FUN_00414580((void *)((int)param_1 + 0xbc),1);
  FUN_0044fcd0(puVar3);
  FUN_0044fcd0((undefined4 *)((int)param_1 + 0xcc));
  for (local_c = 0; local_c < 10; local_c = local_c + 1) {
    uVar4 = 0;
    this = (void *)FUN_00414580((void *)((int)param_1 + 0xd0),local_c);
    FUN_004117a0(this,uVar4);
  }
  for (local_10 = 0; local_10 < 10; local_10 = local_10 + 1) {
    puVar3 = (undefined4 *)FUN_00414580((void *)((int)param_1 + 0x94),local_10);
    FUN_0044fcd0(puVar3);
  }
  FUN_0049c720(param_1,0);
  *(undefined4 *)((int)param_1 + 0x21c) = 0;
  *(undefined4 *)((int)param_1 + 0x270) = 0;
  *(undefined4 *)((int)param_1 + 0x2c4) = 0;
  *(uint *)((int)param_1 + 0x1a4) = *(uint *)((int)param_1 + 0x1a4) | 0xe0;
  return;
}

