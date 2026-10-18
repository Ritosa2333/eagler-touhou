/* Entry: 0x00542a5a; symbol: FUN_00542a5a; body bytes: 251 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00542a5a(void)

{
  code *pcVar1;
  uint uVar2;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 uVar3;
  undefined4 extraout_EDX;
  undefined4 unaff_EBX;
  undefined4 unaff_EBP;
  undefined4 unaff_ESI;
  undefined4 unaff_EDI;
  undefined2 in_ES;
  undefined2 in_CS;
  undefined2 in_SS;
  undefined2 in_DS;
  undefined2 in_FS;
  undefined2 in_GS;
  byte bVar4;
  byte bVar5;
  byte in_AF;
  byte bVar6;
  byte bVar7;
  byte in_TF;
  byte in_IF;
  byte bVar8;
  byte in_NT;
  byte in_AC;
  byte in_VIF;
  byte in_VIP;
  byte in_ID;
  undefined8 uVar9;
  undefined4 unaff_retaddr;
  
  uVar2 = IsProcessorFeaturePresent(0x17);
  uVar9 = CONCAT44(extraout_EDX,uVar2);
  bVar4 = 0;
  bVar8 = 0;
  bVar7 = (int)uVar2 < 0;
  bVar6 = uVar2 == 0;
  bVar5 = (POPCOUNT(uVar2 & 0xff) & 1U) == 0;
  uVar3 = extraout_ECX;
  if (!(bool)bVar6) {
    pcVar1 = (code *)swi(0x29);
    uVar9 = (*pcVar1)();
    uVar3 = extraout_ECX_00;
  }
  _DAT_005e5060 = (undefined4)((ulonglong)uVar9 >> 0x20);
  _DAT_005e5068 = (undefined4)uVar9;
  _DAT_005e5078 =
       (uint)(in_NT & 1) * 0x4000 | (uint)(bVar8 & 1) * 0x800 | (uint)(in_IF & 1) * 0x200 |
       (uint)(in_TF & 1) * 0x100 | (uint)(bVar7 & 1) * 0x80 | (uint)(bVar6 & 1) * 0x40 |
       (uint)(in_AF & 1) * 0x10 | (uint)(bVar5 & 1) * 4 | (uint)(bVar4 & 1) |
       (uint)(in_ID & 1) * 0x200000 | (uint)(in_VIP & 1) * 0x100000 | (uint)(in_VIF & 1) * 0x80000 |
       (uint)(in_AC & 1) * 0x40000;
  _DAT_005e507c = &stack0x00000004;
  _DAT_005e4fb8 = 0x10001;
  _DAT_005e4f68 = 0xc0000409;
  _DAT_005e4f6c = 1;
  _DAT_005e4f78 = 1;
  DAT_005e4f7c = 2;
  _DAT_005e4f74 = unaff_retaddr;
  _DAT_005e5044 = in_GS;
  _DAT_005e5048 = in_FS;
  _DAT_005e504c = in_ES;
  _DAT_005e5050 = in_DS;
  _DAT_005e5054 = unaff_EDI;
  _DAT_005e5058 = unaff_ESI;
  _DAT_005e505c = unaff_EBX;
  _DAT_005e5064 = uVar3;
  _DAT_005e506c = unaff_EBP;
  DAT_005e5070 = unaff_retaddr;
  _DAT_005e5074 = in_CS;
  _DAT_005e5080 = in_SS;
  FUN_00542a32((_EXCEPTION_POINTERS *)&PTR_DAT_00597128);
  return;
}

