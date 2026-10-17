/* Entry: 0x004e60c0; symbol: FUN_004e60c0; body bytes: 363 */

void __fastcall FUN_004e60c0(int param_1)

{
  undefined4 *puVar1;
  undefined4 local_4c;
  int local_48;
  int local_44;
  undefined4 local_40;
  undefined4 local_3c;
  int local_38;
  int local_34;
  undefined *local_30;
  int local_2c;
  int local_28;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_34 = param_1;
  FUN_0044fcd0((undefined4 *)(param_1 + 0xcc));
  local_3c = FUN_00470670();
  puVar1 = FUN_0049e080(&local_4c,(uint *)&DAT_0056fe1c,0x57,(undefined4 *)0x0);
  *(undefined4 *)(local_34 + 0xcc) = *puVar1;
  local_38 = FUN_0044ced0((undefined4 *)(local_34 + 0xcc));
  local_2c = FUN_00437cd0(local_38);
  local_28 = (int)*(float *)(local_2c + 0xc);
  local_24 = (int)*(float *)(local_2c + 0x10);
  local_20 = (int)((*(float *)(local_2c + 0xc) + *(float *)(local_2c + 0x4c)) - DAT_0056c8cc);
  local_1c = (int)((*(float *)(local_2c + 0x10) + *(float *)(local_2c + 0x48)) - DAT_0056c8cc);
  local_30 = &DAT_005c5124;
  local_18 = DAT_005c5228;
  local_14 = DAT_005c522c;
  local_10 = DAT_005c5228 + DAT_005c5230;
  local_c = DAT_005c522c + DAT_005c5234;
  local_40 = DAT_005c4ee0;
  local_44 = FUN_004e6740(local_38);
  puVar1 = (undefined4 *)FUN_00470670();
  local_48 = FUN_0040c300(puVar1);
  FUN_0044be40(DAT_005c0028,local_48,local_44,local_40,&local_28,&local_18,1);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

