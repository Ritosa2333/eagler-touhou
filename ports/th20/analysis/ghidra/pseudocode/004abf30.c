/* Entry: 0x004abf30; symbol: FUN_004abf30; body bytes: 333 */

void __fastcall FUN_004abf30(int param_1)

{
  float fVar1;
  char cVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  size_t sVar6;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar3 = FUN_004ac0c0(0x5c4d40);
  if (iVar3 == 0xf) {
LAB_004abf61:
    FUN_00498f40();
    cVar2 = FUN_004ac160();
    if (cVar2 == '\0') goto LAB_004ac06a;
  }
  else {
    iVar3 = FUN_004ac0c0(0x5c4d40);
    if (iVar3 == 4) goto LAB_004abf61;
  }
  fVar1 = *(float *)(param_1 + 0x38);
  if (DAT_005c0698 != (void *)0x0) {
    if (DAT_0056f280 <= fVar1) {
      if (DAT_00570560 <= fVar1) {
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
    uVar4 = DAT_00570620;
    FUN_00422dd0(local_14,DAT_00570620,DAT_0057061c,0);
    uVar5 = 0x4ac016;
    FUN_00488b00(DAT_005c0698,1);
    sVar6 = 1;
    FUN_0040ddd0(&stack0xffffffd0,"fps");
    FUN_0046ce00(DAT_005c0698,local_14,(float)((double)fVar1 + DAT_0056ff80),uVar4,uVar5,sVar6);
    FUN_00470ad0(DAT_005c0698,0xffffffff);
    FUN_00488b00(DAT_005c0698,0);
  }
LAB_004ac06a:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

