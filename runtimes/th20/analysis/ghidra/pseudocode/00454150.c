/* Entry: 0x00454150; symbol: FUN_00454150; body bytes: 215 */

void __cdecl FUN_00454150(void *param_1,wchar_t *param_2)

{
  undefined4 uVar1;
  int *unaff_FS_OFFSET;
  undefined4 local_418;
  wchar_t local_414 [512];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005687d0;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,3);
  FUN_0040b730(&local_418,uVar1);
  local_8 = 0;
  _memset(local_414,0,0x400);
  FUN_00452d60(local_414,param_2,&stack0x0000000c);
  FUN_004530a0(param_1,(char *)local_414);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_418);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

