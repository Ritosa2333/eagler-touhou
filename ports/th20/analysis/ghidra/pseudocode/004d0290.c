/* Entry: 0x004d0290; symbol: FUN_004d0290; body bytes: 226 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_004d0290(void *param_1)

{
  float *pfVar1;
  float10 fVar2;
  
  FUN_00470af0((void *)((int)param_1 + 0x74c),(undefined4 *)((int)param_1 + 0x58));
  fVar2 = (float10)FUN_00438600(*(float *)((int)param_1 + 0x70),DAT_0056e0f0 / DAT_0056c8d0);
  FUN_00450590((void *)((int)param_1 + 0x74c),(float)fVar2);
  FUN_0044c570((void *)((int)param_1 + 0x74c));
  pfVar1 = (float *)FUN_0044cb60((int)param_1 + 0x1314);
  FUN_004d6bd0(param_1,pfVar1);
  FUN_0044c570((void *)((int)param_1 + 0x1314));
  if (*(float *)((int)param_1 + 0x80) == _DAT_0056e0e8) {
    FUN_00470af0((void *)((int)param_1 + 0xd30),(undefined4 *)((int)param_1 + 0x58));
    FUN_0044c570((void *)((int)param_1 + 0xd30));
  }
  return 0;
}

