/* Entry: 0x0051cf20; symbol: FUN_0051cf20; body bytes: 278 */

/* WARNING: Removing unreachable block (ram,0x0051cf61) */
/* WARNING: Removing unreachable block (ram,0x0051cf71) */

undefined8 __thiscall FUN_0051cf20(void *this,uint *param_1,uint param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  undefined1 uVar3;
  uint *puVar4;
  int _FileHandle;
  int iVar5;
  bool bVar6;
  longlong lVar7;
  uint uVar8;
  int iVar9;
  uint local_14;
  int local_10;
  
  iVar2 = param_3;
  uVar1 = param_2;
  while ((-1 < param_3 && ((0 < param_3 || (param_2 != 0))))) {
    lVar7 = Gnavail();
    if (lVar7 < 1) {
                    /* WARNING: Load size is inaccurate */
      _FileHandle = (**(code **)(*this + 0x1c))();
      iVar9 = _FileHandle;
      iVar5 = eof(_FileHandle);
      bVar6 = FUN_0051b5b0(iVar5,iVar9);
      if (bVar6) break;
      uVar3 = FUN_004ed5a0((char)_FileHandle);
      *(undefined1 *)param_1 = uVar3;
      param_1 = (uint *)((int)param_1 + 1);
      bVar6 = param_2 == 0;
      param_2 = param_2 - 1;
      param_3 = param_3 - (uint)bVar6;
    }
    else {
      if (CONCAT44(param_3,param_2) < lVar7) {
        lVar7 = CONCAT44(param_3,param_2);
      }
      local_10 = (int)((ulonglong)lVar7 >> 0x20);
      local_14 = (uint)lVar7;
      uVar8 = local_14;
      puVar4 = (uint *)FUN_0051bab0((int)this);
      FUN_0040ffd0(param_1,puVar4,uVar8);
      param_1 = (uint *)(local_14 + (int)param_1);
      bVar6 = param_2 < local_14;
      param_2 = param_2 - local_14;
      param_3 = (param_3 - local_10) - (uint)bVar6;
      std::basic_streambuf<char,struct_std::char_traits<char>_>::gbump
                ((basic_streambuf<char,struct_std::char_traits<char>_> *)this,local_14);
    }
  }
  return CONCAT44((iVar2 - param_3) - (uint)(uVar1 < param_2),uVar1 - param_2);
}

