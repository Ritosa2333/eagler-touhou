/* Entry: 0x0051c4f0; symbol: FUN_0051c4f0; body bytes: 439 */

void * __thiscall FUN_0051c4f0(void *this,void *param_1)

{
  bool bVar1;
  bool bVar2;
  int iVar3;
  char *pcVar4;
  uint uVar5;
  uint uVar6;
  undefined8 uVar7;
  uint in_stack_00000020;
  uint local_2c;
  uint local_28;
  uint local_18;
  
  if (((in_stack_00000020 & 1) == 0) || ((*(uint *)((int)this + 0x3c) & 4) == 0)) {
    bVar1 = false;
  }
  else {
    bVar1 = true;
  }
  if (((in_stack_00000020 & 2) == 0) || ((*(uint *)((int)this + 0x3c) & 2) == 0)) {
    bVar2 = false;
  }
  else {
    bVar2 = true;
  }
  if ((bVar1) || (bVar2)) {
    FUN_00515b10(param_1,0xffffffff,0xffffffff);
  }
  else {
    uVar7 = FUN_005163f0((uint *)&stack0x00000008);
    iVar3 = FUN_0051bab0((int)this);
    if ((*(uint *)((int)this + 0x3c) & 2) == 0) {
      local_18 = FUN_0051c110((int)this);
    }
    else {
      local_18 = 0;
    }
    if ((local_18 != 0) && (*(uint *)((int)this + 0x38) < local_18)) {
      *(uint *)((int)this + 0x38) = local_18;
    }
    pcVar4 = (char *)FUN_0051b4c0((int)this);
    uVar5 = *(int *)((int)this + 0x38) - (int)pcVar4;
    uVar6 = (int)uVar5 >> 0x1f;
    local_28 = (uint)((ulonglong)uVar7 >> 0x20);
    local_2c = (uint)uVar7;
    if ((local_28 < uVar6) || ((local_28 <= uVar6 && (local_2c <= uVar5)))) {
      if ((local_2c == 0 && local_28 == 0) ||
         ((((in_stack_00000020 & 1) == 0 || (iVar3 != 0)) &&
          (((in_stack_00000020 & 2) == 0 || (local_18 != 0)))))) {
        if (((in_stack_00000020 & 1) != 0) && (iVar3 != 0)) {
          std::basic_streambuf<char,struct_std::char_traits<char>_>::setg
                    ((basic_streambuf<char,struct_std::char_traits<char>_> *)this,pcVar4,
                     pcVar4 + local_2c,*(char **)((int)this + 0x38));
        }
        if (((in_stack_00000020 & 2) != 0) && (local_18 != 0)) {
          iVar3 = FUN_0051b560((int)this);
          FUN_0051ca70(this,pcVar4,(int)(pcVar4 + local_2c),iVar3);
        }
        FUN_00515b10(param_1,local_2c,local_28);
      }
      else {
        FUN_00515b10(param_1,0xffffffff,0xffffffff);
      }
    }
    else {
      FUN_00515b10(param_1,0xffffffff,0xffffffff);
    }
  }
  return param_1;
}

