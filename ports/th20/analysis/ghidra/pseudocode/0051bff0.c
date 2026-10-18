/* Entry: 0x0051bff0; symbol: FUN_0051bff0; body bytes: 221 */

void __thiscall FUN_0051bff0(void *this,uint param_1)

{
  char cVar1;
  bool bVar2;
  char cVar3;
  undefined1 uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  undefined1 *puVar8;
  
  uVar5 = FUN_0051bab0((int)this);
  if ((uVar5 != 0) && (uVar6 = FUN_0051b4c0((int)this), uVar6 < uVar5)) {
    uVar6 = param_1;
    iVar7 = eof(param_1);
    bVar2 = FUN_0051b5b0(iVar7,uVar6);
    if (!bVar2) {
      cVar1 = *(char *)(uVar5 - 1);
      cVar3 = FUN_004ed5a0((char)param_1);
      bVar2 = FUN_0051b580(cVar3,cVar1);
      if ((!bVar2) && ((*(uint *)((int)this + 0x3c) & 2) != 0)) goto LAB_0051c074;
    }
    std::basic_streambuf<char,struct_std::char_traits<char>_>::gbump
              ((basic_streambuf<char,struct_std::char_traits<char>_> *)this,-1);
    uVar5 = param_1;
    iVar7 = eof(param_1);
    bVar2 = FUN_0051b5b0(iVar7,uVar5);
    if (!bVar2) {
      uVar4 = FUN_004ed5a0((char)param_1);
      puVar8 = (undefined1 *)FUN_0051bab0((int)this);
      *puVar8 = uVar4;
    }
    FUN_0051bd60(param_1);
    return;
  }
LAB_0051c074:
  eof(uVar5);
  return;
}

