/* Entry: 0x00428c90; symbol: FUN_00428c90; body bytes: 323 */

void __thiscall FUN_00428c90(void *this,char **param_1,undefined4 param_2,int param_3)

{
  undefined4 uVar1;
  int *piVar2;
  undefined4 *puVar3;
  int iVar4;
  int *unaff_FS_OFFSET;
  int iVar5;
  int local_1c;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056824d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,0xb);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  local_1c = 0;
  do {
    if (0x1e < local_1c) {
LAB_00428d9c:
      std::_Adl_verify_range<char*,char_const*>((char **)"Sound Que Add %d\r\n",param_1);
      local_8 = 0xffffffff;
      FUN_0040b900(&local_18);
      *unaff_FS_OFFSET = local_10;
      FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
      return;
    }
    piVar2 = (int *)FUN_004260d0((void *)((int)this + 0x2544),local_1c);
    if (*piVar2 == 0) {
      puVar3 = (undefined4 *)FUN_004260d0((void *)((int)this + 0x2544),local_1c);
      *puVar3 = param_1;
      iVar4 = FUN_004260d0((void *)((int)this + 0x2544),local_1c);
      *(undefined4 *)(iVar4 + 4) = param_2;
      iVar5 = 0x100;
      iVar4 = FUN_004260d0((void *)((int)this + 0x2544),local_1c);
      FUN_00548610((char *)(iVar4 + 0xc),iVar5,param_3);
      iVar4 = FUN_004260d0((void *)((int)this + 0x2544),local_1c);
      *(undefined4 *)(iVar4 + 8) = 0;
      goto LAB_00428d9c;
    }
    local_1c = local_1c + 1;
  } while( true );
}

