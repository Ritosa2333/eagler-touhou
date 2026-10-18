/* Entry: 0x00553735; symbol: operator(); body bytes: 218 */

/* Library Function - Single Match
    public: int __thiscall <lambda_f03950bc5685219e0bcd2087efbe011e>::operator()(void)const 
   
   Library: Visual Studio 2019 Release */

int __thiscall
<lambda_f03950bc5685219e0bcd2087efbe011e>::operator()
          (<lambda_f03950bc5685219e0bcd2087efbe011e> *this)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  sbyte sVar5;
  uint uVar6;
  uint *puVar7;
  uint *puVar8;
  uint *puVar9;
  uint *puVar10;
  
  uVar1 = DAT_005b25c0;
  puVar9 = (uint *)**(int **)this;
  if (puVar9 == (uint *)0x0) {
    iVar2 = -1;
  }
  else {
    uVar6 = DAT_005b25c0 & 0x1f;
    sVar5 = (sbyte)uVar6;
    puVar8 = (uint *)((*puVar9 ^ DAT_005b25c0) >> sVar5 | (*puVar9 ^ DAT_005b25c0) << 0x20 - sVar5);
    puVar9 = (uint *)((puVar9[1] ^ DAT_005b25c0) >> sVar5 |
                     (puVar9[1] ^ DAT_005b25c0) << 0x20 - sVar5);
    if ((puVar8 != (uint *)0x0) &&
       (uVar3 = DAT_005b25c0, puVar10 = puVar9, puVar8 != (uint *)0xffffffff)) {
      while (puVar9 = puVar9 + -1, puVar8 <= puVar9) {
        if (*puVar9 != uVar1) {
          uVar3 = *puVar9 ^ uVar3;
          *puVar9 = uVar1;
          (*(code *)PTR_guard_check_icall_0056c37c)();
          (*(code *)(uVar3 >> (sbyte)uVar6 | uVar3 << 0x20 - (sbyte)uVar6))();
          uVar6 = DAT_005b25c0 & 0x1f;
          uVar3 = *(uint *)**(int **)this ^ DAT_005b25c0;
          sVar5 = (sbyte)uVar6;
          puVar7 = (uint *)(uVar3 >> sVar5 | uVar3 << 0x20 - sVar5);
          uVar3 = ((uint *)**(int **)this)[1] ^ DAT_005b25c0;
          puVar4 = (uint *)(uVar3 >> sVar5 | uVar3 << 0x20 - sVar5);
          uVar3 = DAT_005b25c0;
          if ((puVar7 != puVar8) || (puVar4 != puVar10)) {
            puVar9 = puVar4;
            puVar10 = puVar4;
            puVar8 = puVar7;
          }
        }
      }
      if (puVar8 != (uint *)0xffffffff) {
        FUN_00557ba0(puVar8);
        uVar3 = DAT_005b25c0;
      }
      *(uint *)**(undefined4 **)this = uVar3;
      *(uint *)(**(int **)this + 4) = uVar3;
      *(uint *)(**(int **)this + 8) = uVar3;
    }
    iVar2 = 0;
  }
  return iVar2;
}

