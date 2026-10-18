/* Entry: 0x00510e80; symbol: FUN_00510e80; body bytes: 1004 */

void __fastcall FUN_00510e80(int param_1)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  float local_174 [3];
  float local_168 [3];
  float local_15c [3];
  float local_150 [3];
  float local_144 [3];
  float local_138 [3];
  undefined4 local_12c;
  undefined4 local_128;
  undefined4 local_124;
  undefined4 local_120;
  int local_11c;
  undefined1 local_115;
  undefined1 local_114 [12];
  char local_108 [256];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_11c = param_1;
  bVar1 = FUN_0047a340((undefined4 *)(param_1 + 0x14));
  if ((bVar1) && (uVar2 = FUN_00511570(DAT_005ba828), (uVar2 & 0xff) == 0)) {
    FUN_00422dd0(local_114,DAT_0056ed14,DAT_0056ec98,0);
    FUN_004e67e0(DAT_005c0698);
    iVar3 = FUN_0044ced0((undefined4 *)(local_11c + 0x14));
    iVar3 = FUN_0045d0e0(iVar3);
    local_115 = *(undefined1 *)(iVar3 + 3);
    FUN_00488940(DAT_005c0698,local_115);
    FUN_004c6180(DAT_005c0698,4);
    FUN_004b8620(DAT_005c0698,0,0);
    FUN_00422dd0(local_138,DAT_0056ed14,DAT_0056ec98,0);
    FUN_0040ddd0(&stack0xfffffe84,&DAT_00573f50);
    FUN_0046c210(DAT_005c0698,local_138);
    iVar3 = FUN_00464080(0);
    local_120 = FUN_005113c0(iVar3);
    FUN_00422dd0(local_144,DAT_0056ed14,DAT_00573fdc,0);
    FUN_0040ddd0(&stack0xfffffe80,&DAT_00573f64);
    FUN_0046c210(DAT_005c0698,local_144);
    iVar3 = FUN_00464080(0);
    local_124 = FUN_00511320(iVar3);
    FUN_00422dd0(local_150,DAT_0056ed14,DAT_00573fe0,0);
    FUN_0040ddd0(&stack0xfffffe80,&DAT_00573f7c);
    FUN_0046c210(DAT_005c0698,local_150);
    iVar3 = FUN_00464080(0);
    local_128 = FUN_00511410(iVar3);
    FUN_00422dd0(local_15c,DAT_0056ed14,DAT_00573fe4,0);
    FUN_0040ddd0(&stack0xfffffe80,&DAT_00573f94);
    FUN_0046c210(DAT_005c0698,local_15c);
    iVar3 = FUN_00464080(0);
    local_12c = FUN_00511370(iVar3);
    FUN_00422dd0(local_168,DAT_0056ed14,DAT_00573fe8,0);
    FUN_0040ddd0(&stack0xfffffe80,&DAT_00573fac);
    FUN_0046c210(DAT_005c0698,local_168);
    FUN_00453500(local_108,0x100,(longlong)*(int *)(local_11c + 0x28));
    FUN_00422dd0(local_174,DAT_0056ed14,DAT_0056f10c,0);
    FUN_0040ddd0(&stack0xfffffe80,&DAT_00573fc4);
    FUN_0046c210(DAT_005c0698,local_174);
    FUN_00488b00(DAT_005c0698,4);
    FUN_004e67e0(DAT_005c0698);
  }
  if (DAT_005ba828 != 0) {
    uVar2 = FUN_00511570(DAT_005ba828);
    if (((uVar2 & 0xff) == 0) && (*(int *)(local_11c + 0x24) == 4)) {
      FUN_0044fa30();
    }
    else {
      FUN_00450120();
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

