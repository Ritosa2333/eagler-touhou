/* Entry: 0x004b9740; symbol: FUN_004b9740; body bytes: 626 */

void __thiscall FUN_004b9740(void *this,int param_1)

{
  bool bVar1;
  int iVar2;
  size_t sVar3;
  char *_Str1;
  uint uVar4;
  undefined3 extraout_var;
  undefined4 *puVar5;
  char *_Str2;
  undefined4 local_120;
  undefined4 local_11c;
  uint local_118;
  uint local_114;
  char *local_110;
  void *local_10c;
  char local_108 [256];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_10c = this;
  if ((param_1 == -1) || (param_1 == -3)) {
    local_118 = (uint)(param_1 == -1);
    local_114 = local_118;
    bVar1 = FUN_00488850(0x5ba568);
    if ((bVar1) && (iVar2 = FUN_00488830(DAT_005ba828), iVar2 == 0)) {
      FUN_00548610(local_108,0x100,*(int *)(DAT_005c6110 + 0xc + local_114 * 4));
      sVar3 = _strlen(local_108);
      local_110 = local_108 + sVar3;
      builtin_strncpy(local_108 + sVar3,".wav",5);
      _Str2 = local_108;
      _Str1 = (char *)FUN_004b8090(0x5ba830);
      iVar2 = _strcmp(_Str1,_Str2);
      if (iVar2 == 0) goto LAB_004b99a2;
    }
    FUN_004d9b50(local_114,*(int *)(DAT_005c6110 + 0x58 + local_114 * 4));
    local_11c = FUN_004b8270(DAT_005c06a4);
    FUN_00450c70(&local_120,(uint *)0x0,local_114 + 1,-1,(undefined4 *)0x0);
  }
  else if (param_1 == -2) {
    iVar2 = FUN_00478ea0(0);
    uVar4 = FUN_00488880(iVar2);
    if (uVar4 == 0) {
      FUN_004bc570(DAT_005ba828);
    }
    else {
      FUN_004e5bd0(DAT_005c60bc);
    }
  }
  else {
    bVar1 = FUN_00478160((int)this);
    if (CONCAT31(extraout_var,bVar1) != 0) {
      FUN_004ae830(*(void **)((int)local_10c + 0x1bc));
      *(undefined4 *)((int)local_10c + 0x1bc) = 0;
      *(undefined4 *)((int)local_10c + 0x1bc) = 0;
    }
    puVar5 = FUN_004ae930();
    *(undefined4 **)((int)local_10c + 0x1bc) = puVar5;
    **(int **)((int)local_10c + 0x1bc) = param_1;
  }
LAB_004b99a2:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

