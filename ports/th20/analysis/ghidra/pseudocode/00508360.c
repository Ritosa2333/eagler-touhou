/* Entry: 0x00508360; symbol: FUN_00508360; body bytes: 278 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00508360(int param_1)

{
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (((DAT_005ba828 != 0) && (*(int *)(param_1 + 0x10) != 0)) && (*(int *)(param_1 + 0x10) == 1)) {
    FUN_00422dd0(local_14,DAT_00573708,_DAT_0057370c,0);
    if (DAT_0056f280 <= (float)*(byte *)(param_1 + 0x24c)) {
      if (DAT_0056ec90 <= (float)*(byte *)(param_1 + 0x24c)) {
        local_1c = 0xffffffff;
      }
      else {
        local_1c = 0xffa0a0ff;
      }
      local_20 = local_1c;
    }
    else {
      local_20 = 0xff5050ff;
    }
    FUN_00470ad0(DAT_005c0698,local_20);
    FUN_0046c990(DAT_005c0698,local_14,
                 L"㌥d㩄捜杹楷屮潨敭穜湵灜潲屧桴〲獜捲杜浡履敲汰祡挮灰㔺〳瘠楯d"
                );
    FUN_00470ad0(DAT_005c0698,0xffffffff);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

