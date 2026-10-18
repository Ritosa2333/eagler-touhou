/* Entry: 0x004feb80; symbol: FUN_004feb80; body bytes: 121 */

void __fastcall FUN_004feb80(undefined4 *param_1)

{
  char **ppcVar1;
  char **ppcVar2;
  int iVar3;
  undefined1 local_20 [4];
  undefined1 local_1c [4];
  undefined4 *local_18;
  int local_14;
  undefined4 *local_10;
  undefined4 *local_c;
  undefined4 *local_8;
  
  local_c = param_1;
  local_8 = param_1;
  ppcVar1 = (char **)FUN_004fefc0(param_1,local_1c);
  std::_Adl_verify_range<char*,char_const*>((char **)&stack0x00000004,ppcVar1);
  local_10 = local_8;
  ppcVar1 = (char **)&stack0x00000004;
  ppcVar2 = (char **)FUN_004fed50(local_8,local_20);
  std::_Adl_verify_range<char*,char_const*>(ppcVar2,ppcVar1);
  iVar3 = FUN_0040c300((undefined4 *)&stack0x00000004);
  local_14 = FUN_0040c300(local_8);
  local_14 = iVar3 - local_14;
  local_18 = local_8;
  FUN_004ffc80(local_8,local_14);
  return;
}

