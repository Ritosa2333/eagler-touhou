/* Entry: 0x00555c78; symbol: operator(); body bytes: 254 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Library Function - Single Match
    public: void __thiscall <lambda_b57350f2640456a0859d250846f69caf>::operator()(void)const 
   
   Library: Visual Studio 2019 Release */

void __thiscall
<lambda_b57350f2640456a0859d250846f69caf>::operator()
          (<lambda_b57350f2640456a0859d250846f69caf> *this)

{
  ushort uVar1;
  int iVar2;
  ushort *puVar3;
  uint uVar4;
  ushort *puVar5;
  bool bVar6;
  
  __copytlocinfo_nolock
            ((undefined4 *)**(undefined4 **)this,*(undefined4 **)(**(int **)(this + 4) + 0x4c));
  iVar2 = __wsetlocale_nolock(**(int **)this,**(int **)(this + 0xc),
                              (wchar_t *)**(undefined4 **)(this + 0x10));
  **(int **)(this + 8) = iVar2;
  if (iVar2 == 0) {
    FUN_00560b07(**(int **)this);
    FUN_0056093c((LPVOID)**(undefined4 **)this);
  }
  else {
    puVar3 = (ushort *)**(undefined4 **)(this + 0x10);
    if (puVar3 != (ushort *)0x0) {
      puVar5 = &DAT_005b2848;
      do {
        uVar1 = *puVar3;
        bVar6 = uVar1 < *puVar5;
        if (uVar1 != *puVar5) {
LAB_00555ce9:
          uVar4 = -(uint)bVar6 | 1;
          goto LAB_00555ced;
        }
        if (uVar1 == 0) break;
        uVar1 = puVar3[1];
        bVar6 = uVar1 < puVar5[1];
        if (uVar1 != puVar5[1]) goto LAB_00555ce9;
        puVar3 = puVar3 + 2;
        puVar5 = puVar5 + 2;
      } while (uVar1 != 0);
      uVar4 = 0;
LAB_00555ced:
      if (uVar4 != 0) {
        LOCK();
        DAT_005e55dc = 1;
        UNLOCK();
      }
    }
    __updatetlocinfoEx_nolock
              ((undefined4 *)(**(int **)(this + 4) + 0x4c),(undefined **)**(undefined4 **)this);
    FUN_00560b07(**(int **)this);
    if (((*(byte *)(**(int **)(this + 4) + 0x350) & 2) == 0) && (((byte)DAT_005b29f8 & 1) == 0)) {
      __updatetlocinfoEx_nolock(&DAT_005e5664,*(undefined ***)(**(int **)(this + 4) + 0x4c));
      PTR_PTR_005b26b0 = (undefined *)DAT_005e5664[0x22];
      PTR_DAT_005b2770 = (undefined *)*DAT_005e5664;
      _DAT_005b284c = DAT_005e5664[1];
      return;
    }
  }
  return;
}

