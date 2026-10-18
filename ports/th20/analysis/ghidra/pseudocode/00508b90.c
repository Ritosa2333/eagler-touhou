/* Entry: 0x00508b90; symbol: FUN_00508b90; body bytes: 981 */

void __thiscall FUN_00508b90(void *this,undefined1 (*param_1) [16])

{
  int iVar1;
  char cVar2;
  char **ppcVar3;
  char **ppcVar4;
  uint uVar5;
  int iVar6;
  undefined4 uVar7;
  undefined4 *puVar8;
  int *piVar9;
  int iVar10;
  int *unaff_FS_OFFSET;
  int local_60;
  int local_58;
  uint *local_54;
  int *local_50;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_48 [24];
  function<void___cdecl(void)> local_30 [24];
  SIZE_T local_18;
  char **local_14;
  int local_10;
  undefined1 *puStack_c;
  uint local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a7a5;
  local_10 = *unaff_FS_OFFSET;
  ppcVar3 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = ppcVar3;
  FUN_00548610((char *)((int)this + 0x260),0x100,(int)param_1);
  FUN_00417cd0(local_30,&DAT_005b67e1);
  local_8 = 0;
  FUN_00417e40("replay");
  FUN_004d8620(&param_1);
  FUN_0041e790(local_30,local_48);
  local_8 = CONCAT31(local_8._1_3_,1);
  ppcVar4 = (char **)FUN_0040ff30((undefined4 *)local_48);
  uVar5 = FUN_004b8390(0x5ba568);
  if (uVar5 == 0) {
    cVar2 = FUN_004107a0();
    if ((cVar2 != '\0') && (iVar6 = FUN_00410cf0(ppcVar4), iVar6 == 0)) {
      uVar7 = FUN_00410990(0x30);
      *(undefined4 *)((int)this + 0x18) = uVar7;
      if (((**(int **)((int)this + 0x18) == 0x72303274) &&
          (((*(short *)(*(int *)((int)this + 0x18) + 4) == 1 &&
            (*(int *)(*(int *)((int)this + 0x18) + 0x1c) == 0x30)) &&
           (*(int *)(*(int *)((int)this + 0x18) + 0x20) == 0x100)))) &&
         (*(int *)(*(int *)((int)this + 0x18) + 0x24) == 0x2a0)) {
        local_54 = (uint *)FUN_00410990(*(SIZE_T *)(*(int *)((int)this + 0x18) + 0x28));
        FUN_004106e0();
        goto LAB_00508d1c;
      }
      std::_Adl_verify_range<char*,char_const*>((char **)&DAT_00573638,ppcVar3);
      FUN_004106e0();
    }
    local_8 = local_8 & 0xffffff00;
    FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)local_48);
    local_8 = 0xffffffff;
    FUN_0040e6c0(local_30);
  }
  else {
    uVar7 = FUN_00410aa0(param_1,&local_18,0);
    *(undefined4 *)((int)this + 0x18) = uVar7;
    local_54 = (uint *)(*(int *)((int)this + 0x18) + 0x30);
LAB_00508d1c:
    uVar7 = FUN_0041f610(*(SIZE_T *)(*(int *)((int)this + 0x18) + 0x2c));
    *(undefined4 *)((int)this + 0x248) = uVar7;
    FUN_004100e0(local_54,*(uint *)(*(int *)((int)this + 0x18) + 0x28),0x5c,-0x1f,0x400,
                 *(uint *)(*(int *)((int)this + 0x18) + 0x28));
    FUN_004100e0(local_54,*(uint *)(*(int *)((int)this + 0x18) + 0x28),0x7d,':',0x100,
                 *(uint *)(*(int *)((int)this + 0x18) + 0x28));
    FUN_005391f0((byte *)local_54,*(int *)(*(int *)((int)this + 0x18) + 0x28),
                 *(undefined1 **)((int)this + 0x248),*(SIZE_T *)(*(int *)((int)this + 0x18) + 0x2c))
    ;
    *(undefined4 *)((int)this + 0x1c) = *(undefined4 *)((int)this + 0x248);
    local_50 = (int *)(*(int *)((int)this + 0x1c) + 0x100);
    local_58 = 0;
    while( true ) {
      if (*(int *)(*(int *)((int)this + 0x1c) + 0xd4) < 8) {
        local_60 = *(int *)(*(int *)((int)this + 0x1c) + 0xd4);
      }
      else {
        local_60 = 6;
      }
      if (local_60 <= local_58) break;
      iVar6 = FUN_00486e30((void *)((int)this + 0xe8),*local_50);
      *(int **)(iVar6 + 0x10) = local_50;
      puVar8 = (undefined4 *)FUN_00486e30((void *)((int)this + 0xe8),*local_50);
      *puVar8 = local_50 + 0xa8;
      piVar9 = (int *)FUN_00486e30((void *)((int)this + 0xe8),*local_50);
      iVar6 = *piVar9;
      iVar1 = local_50[2];
      iVar10 = FUN_00486e30((void *)((int)this + 0xe8),*local_50);
      *(int *)(iVar10 + 8) = iVar6 + iVar1 * 6;
      local_50 = (int *)((int)local_50 + local_50[3] + 0x2a0);
      local_58 = local_58 + 1;
    }
    uVar5 = FUN_004b8390(0x5ba568);
    if ((uVar5 == 0) && (local_54 != (uint *)0x0)) {
      FUN_0041f670(local_54);
    }
    local_8 = local_8 & 0xffffff00;
    FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)local_48);
    local_8 = 0xffffffff;
    FUN_0040e6c0(local_30);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee((uint)local_14 ^ (uint)&stack0xfffffffc);
  return;
}

