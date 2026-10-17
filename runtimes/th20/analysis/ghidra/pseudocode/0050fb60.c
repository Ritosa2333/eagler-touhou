/* Entry: 0x0050fb60; symbol: FUN_0050fb60; body bytes: 174 */

void __fastcall FUN_0050fb60(int param_1)

{
  char **ppcVar1;
  undefined4 uVar2;
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  char **local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056762d;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = ppcVar1;
  uVar2 = FUN_0040c310(&DAT_005c0240,0x14);
  FUN_0040b730(&local_18,uVar2);
  local_8 = 0;
  FUN_0050f6b0("scoreth20bak.dat",param_1 + 0x92140);
  FUN_0050f6b0("scoreth20.dat",param_1);
  FUN_0050eeb0(param_1);
  std::_Adl_verify_range<char*,char_const*>((char **)"Score.SaveThread end\n",ppcVar1);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee((uint)local_14 ^ (uint)&stack0xfffffffc);
  return;
}

