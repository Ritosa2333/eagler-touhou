/* Entry: 0x0051bdc0; symbol: FUN_0051bdc0; body bytes: 546 */

void __thiscall FUN_0051bdc0(void *this,uint param_1)

{
  bool bVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  char *pcVar6;
  uint *puVar7;
  undefined4 uVar8;
  uint *puVar9;
  LPVOID pvVar10;
  int in_stack_ffffffc8;
  uint local_18;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if ((*(uint *)((int)this + 0x3c) & 2) == 0) {
    uVar4 = param_1;
    iVar3 = eof(param_1);
    bVar1 = FUN_0051b5b0(iVar3,uVar4);
    if (bVar1) {
      FUN_0051bd60(param_1);
    }
    else {
      uVar4 = FUN_0051c110((int)this);
      uVar5 = FUN_0051b560((int)this);
      if ((uVar4 == 0) || (uVar5 <= uVar4)) {
        local_18 = 0;
        puVar7 = (uint *)FUN_0051b4c0((int)this);
        if (uVar4 != 0) {
          local_18 = uVar5 - (int)puVar7;
        }
        if (local_18 < 0x20) {
          local_c = 0x20;
        }
        else if (local_18 < 0x3fffffff) {
          local_c = local_18 << 1;
        }
        else {
          if (0x7ffffffe < local_18) {
            eof(in_stack_ffffffc8);
            goto LAB_0051bfd2;
          }
          local_c = 0x7fffffff;
        }
        uVar8 = FUN_0040c7c0((allocator<char> *)((int)this + 0x40),&local_c);
        puVar9 = (uint *)move<>(uVar8);
        FUN_0040ffd0(puVar9,puVar7,local_18);
        *(uint *)((int)this + 0x38) = (int)puVar9 + local_18 + 1;
        FUN_0051ca70(this,puVar9,(int)puVar9 + local_18,(int)puVar9 + local_c);
        if ((*(uint *)((int)this + 0x3c) & 4) == 0) {
          pcVar6 = *(char **)((int)this + 0x38);
          iVar3 = FUN_0051bab0((int)this);
          std::basic_streambuf<char,struct_std::char_traits<char>_>::setg
                    ((basic_streambuf<char,struct_std::char_traits<char>_> *)this,(char *)puVar9,
                     (char *)((iVar3 - (int)puVar7) + (int)puVar9),pcVar6);
        }
        else {
          std::basic_streambuf<char,struct_std::char_traits<char>_>::setg
                    ((basic_streambuf<char,struct_std::char_traits<char>_> *)this,(char *)puVar9,
                     (char *)puVar9,(char *)puVar9);
        }
        if ((*(uint *)((int)this + 0x3c) & 1) != 0) {
          pvVar10 = (LPVOID)FUN_0048b180(puVar7);
          FUN_004100a0(pvVar10,local_18);
        }
        *(uint *)((int)this + 0x3c) = *(uint *)((int)this + 0x3c) | 1;
        cVar2 = FUN_004ed5a0((char)param_1);
        pcVar6 = std::basic_streambuf<char,struct_std::char_traits<char>_>::_Pninc
                           ((basic_streambuf<char,struct_std::char_traits<char>_> *)this);
        *pcVar6 = cVar2;
      }
      else {
        cVar2 = FUN_004ed5a0((char)param_1);
        pcVar6 = std::basic_streambuf<char,struct_std::char_traits<char>_>::_Pninc
                           ((basic_streambuf<char,struct_std::char_traits<char>_> *)this);
        *pcVar6 = cVar2;
        *(uint *)((int)this + 0x38) = uVar4 + 1;
      }
    }
  }
  else {
    eof(in_stack_ffffffc8);
  }
LAB_0051bfd2:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

