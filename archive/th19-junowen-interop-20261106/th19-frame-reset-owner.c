// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Reference 004ed134
// Reference 004ed13c
// Reference 00517224
// Reference 004ea586
// Reference 00515de1
// Reference 005168ac
// Address 516d60

undefined4 * __fastcall FUN_00516d60(undefined4 param_1)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 extraout_ECX;
  void *local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  puStack_c = &LAB_005774b7;
  local_10 = ExceptionList;
  uVar1 = DAT_005c3d80 ^ (uint)&stack0xfffffffc;
  ExceptionList = &local_10;
  local_8 = 0;
  puVar2 = (undefined4 *)FUN_004d6b50(0x180,8);
  puVar2[1] = 0;
  puVar2[1] = puVar2[1] | 2;
  puVar2[4] = 0;
  puVar2[5] = 0;
  puVar2[6] = 0;
  puVar2[7] = 0;
  puVar2[2] = 0;
  puVar2[3] = 0;
  *puVar2 = GameInf::vftable;
  puVar2[8] = 0;
  puVar2[9] = 0;
  puVar2[10] = 0;
  puVar2[0xb] = 0;
  puVar2[0xc] = 0;
  puVar2[0xd] = 0;
  puVar2[0xe] = 0;
  puVar2[0xf] = 0;
  FUN_00413170(uVar1);
  puVar2[0x50] = 0;
  puVar2[0x51] = 0;
  puVar2[0x52] = 0;
  puVar2[0x53] = 0;
  puVar2[0x54] = 0;
  puVar2[0x55] = 0;
  puVar2[0x56] = 0;
  puVar2[0x57] = 0;
  puVar2[0x58] = 0;
  puVar2[0x59] = 0;
  puVar2[0x5a] = 0;
  puVar2[0x5b] = 1;
  puVar2[0x5c] = 0;
  puVar2[0x5d] = 0xffff00;
  puVar2[0x5e] = 0;
  puVar2[0x5f] = 0;
  local_8 = 0xffffffff;
  DAT_0062eef0 = 0;
  (**(code **)(*DAT_0062bb48 + 0x14))(DAT_0062bb48);
  puVar2[0x50] = puVar2[0x50] | 4;
  DAT_005d1a54 = puVar2;
  puVar2[0x58] = param_1;
  FUN_00412e70(&LAB_00517790,extraout_ECX);
  ExceptionList = local_10;
  return puVar2;
}


// Reference 00532eea
// Reference 00532f17
// Reference 00532f4c
// Reference 00532f85
// Reference 00532faa
// Reference 00532fe6
// Reference 0053301f
// Reference 00533044
