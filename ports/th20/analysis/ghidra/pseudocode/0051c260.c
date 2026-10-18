/* Entry: 0x0051c260; symbol: FUN_0051c260; body bytes: 621 */

void * __thiscall
FUN_0051c260(void *this,void *param_1,uint param_2,int param_3,int param_4,uint param_5)

{
  bool bVar1;
  bool bVar2;
  int iVar3;
  char *pcVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  uint local_34;
  uint uStack_30;
  uint local_20;
  
  if (((param_5 & 1) == 0) || ((*(uint *)((int)this + 0x3c) & 4) == 0)) {
    bVar1 = false;
  }
  else {
    bVar1 = true;
  }
  if (((param_5 & 2) == 0) || ((*(uint *)((int)this + 0x3c) & 2) == 0)) {
    bVar2 = false;
  }
  else {
    bVar2 = true;
  }
  if ((bVar1) || (bVar2)) {
    FUN_00515b10(param_1,0xffffffff,0xffffffff);
    return param_1;
  }
  iVar3 = FUN_0051bab0((int)this);
  if ((*(uint *)((int)this + 0x3c) & 2) == 0) {
    local_20 = FUN_0051c110((int)this);
  }
  else {
    local_20 = 0;
  }
  if ((local_20 != 0) && (*(uint *)((int)this + 0x38) < local_20)) {
    *(uint *)((int)this + 0x38) = local_20;
  }
  pcVar4 = (char *)FUN_0051b4c0((int)this);
  uVar5 = *(int *)((int)this + 0x38) - (int)pcVar4;
  uVar7 = (int)uVar5 >> 0x1f;
  if (param_4 == 0) {
    local_34 = 0;
    uStack_30 = 0;
LAB_0051c3d3:
    uVar9 = param_3 + uStack_30 + (uint)CARRY4(param_2,local_34);
    if ((uVar9 < uVar7) || ((uVar9 <= uVar7 && (param_2 + local_34 <= uVar5)))) {
      iVar6 = param_2 + local_34;
      iVar8 = param_3 + uStack_30 + (uint)CARRY4(param_2,local_34);
      if ((iVar6 == 0 && iVar8 == 0) ||
         ((((param_5 & 1) == 0 || (iVar3 != 0)) && (((param_5 & 2) == 0 || (local_20 != 0)))))) {
        if (((param_5 & 1) != 0) && (iVar3 != 0)) {
          std::basic_streambuf<char,struct_std::char_traits<char>_>::setg
                    ((basic_streambuf<char,struct_std::char_traits<char>_> *)this,pcVar4,
                     pcVar4 + iVar6,*(char **)((int)this + 0x38));
        }
        if (((param_5 & 2) != 0) && (local_20 != 0)) {
          iVar3 = FUN_0051b560((int)this);
          FUN_0051ca70(this,pcVar4,(int)(pcVar4 + iVar6),iVar3);
        }
        FUN_00515b10(param_1,iVar6,iVar8);
      }
      else {
        FUN_00515b10(param_1,0xffffffff,0xffffffff);
      }
    }
    else {
      FUN_00515b10(param_1,0xffffffff,0xffffffff);
    }
  }
  else {
    if (param_4 == 1) {
      if ((param_5 & 3) != 3) {
        if ((param_5 & 1) == 0) {
          if (((param_5 & 2) != 0) && ((local_20 != 0 || (pcVar4 == (char *)0x0)))) {
            local_34 = local_20 - (int)pcVar4;
            uStack_30 = (int)(local_20 - (int)pcVar4) >> 0x1f;
            goto LAB_0051c3d3;
          }
        }
        else if ((iVar3 != 0) || (pcVar4 == (char *)0x0)) {
          local_34 = iVar3 - (int)pcVar4;
          uStack_30 = iVar3 - (int)pcVar4 >> 0x1f;
          goto LAB_0051c3d3;
        }
      }
    }
    else {
      local_34 = uVar5;
      uStack_30 = uVar7;
      if (param_4 == 2) goto LAB_0051c3d3;
    }
    FUN_00515b10(param_1,0xffffffff,0xffffffff);
  }
  return param_1;
}

