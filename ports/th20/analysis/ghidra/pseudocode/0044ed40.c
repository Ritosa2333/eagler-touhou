/* Entry: 0x0044ed40; symbol: FUN_0044ed40; body bytes: 269 */

void __thiscall FUN_0044ed40(void *this,uint param_1,char **param_2)

{
  undefined4 uVar1;
  uint *puVar2;
  undefined4 *puVar3;
  int iVar4;
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005676ed;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  std::_Adl_verify_range<char*,char_const*>((char **)"::preloadAnim : %s\n",param_2);
  if ((int)param_1 < 0x2a) {
    uVar1 = FUN_0040c310(&DAT_005c0240,0x11);
    FUN_0040b730(&local_18,uVar1);
    local_8 = 0;
    puVar2 = FUN_004477b0();
    puVar3 = (undefined4 *)FUN_0043eb10((void *)((int)this + 0x6000730),param_1);
    *puVar3 = puVar2;
    *puVar2 = param_1;
    local_8 = 0xffffffff;
    FUN_0040b900(&local_18);
    iVar4 = FUN_0044a700(param_2);
    if (iVar4 != 0) {
      FUN_004473a0(puVar2);
    }
  }
  else {
    FUN_00454230(&DAT_005c0678,(wchar_t *)&DAT_0056e38c);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

