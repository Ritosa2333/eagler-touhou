/* Entry: 0x0051d040; symbol: FUN_0051d040; body bytes: 293 */

/* WARNING: Removing unreachable block (ram,0x0051d081) */
/* WARNING: Removing unreachable block (ram,0x0051d091) */

undefined8 __thiscall FUN_0051d040(void *this,uint *param_1,uint param_2,int param_3)

{
  code *pcVar1;
  int iVar2;
  undefined1 uVar3;
  uint *puVar4;
  int iVar5;
  int iVar6;
  bool bVar7;
  longlong lVar8;
  uint *puVar9;
  uint uVar10;
  uint _FileHandle;
  uint local_20;
  int local_1c;
  
  iVar2 = param_3;
  _FileHandle = param_2;
  while ((-1 < param_3 && ((0 < param_3 || (param_2 != 0))))) {
    lVar8 = FUN_0051af10();
    if (lVar8 < 1) {
                    /* WARNING: Load size is inaccurate */
      pcVar1 = *(code **)(*this + 0xc);
      uVar3 = FUN_004ed5a0((char)*param_1);
      iVar5 = (*pcVar1)(uVar3);
      iVar6 = eof(_FileHandle);
      bVar7 = FUN_0051b5b0(iVar6,iVar5);
      if (bVar7) break;
      param_1 = (uint *)((int)param_1 + 1);
      bVar7 = param_2 == 0;
      param_2 = param_2 - 1;
      param_3 = param_3 - (uint)bVar7;
    }
    else {
      if (CONCAT44(param_3,param_2) < lVar8) {
        lVar8 = CONCAT44(param_3,param_2);
      }
      local_1c = (int)((ulonglong)lVar8 >> 0x20);
      local_20 = (uint)lVar8;
      puVar9 = param_1;
      uVar10 = local_20;
      puVar4 = (uint *)FUN_0051c110((int)this);
      FUN_0040ffd0(puVar4,puVar9,uVar10);
      param_1 = (uint *)(local_20 + (int)param_1);
      bVar7 = param_2 < local_20;
      param_2 = param_2 - local_20;
      param_3 = (param_3 - local_1c) - (uint)bVar7;
      FUN_0051c0d0(this,local_20);
    }
  }
  return CONCAT44((iVar2 - param_3) - (uint)(_FileHandle < param_2),_FileHandle - param_2);
}

