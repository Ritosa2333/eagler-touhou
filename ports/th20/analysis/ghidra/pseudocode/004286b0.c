/* Entry: 0x004286b0; symbol: FUN_004286b0; body bytes: 348 */

undefined4 __thiscall FUN_004286b0(void *this,char **param_1)

{
  ushort uVar1;
  char *pcVar2;
  undefined4 uVar3;
  HANDLE pvVar4;
  char **ppcVar5;
  uint uVar6;
  char **in_stack_ffffffe0;
  
  FUN_00548610((char *)((int)this + 0x56c4),0x100,(int)param_1);
  if (*(int *)((int)this + 0xc) == 0) {
    uVar3 = 0xffffffff;
  }
  else {
                    /* WARNING: Load size is inaccurate */
    if (*this == 0) {
      uVar3 = 0xffffffff;
    }
    else {
      std::_Adl_verify_range<char*,char_const*>((char **)"Streming BGM Start\r\n",in_stack_ffffffe0)
      ;
      FUN_00428990((int)this);
      pcVar2 = *(char **)((int)this + 0x1890);
      uVar1 = *(ushort *)(pcVar2 + 0x2c);
      uVar6 = *(int *)(pcVar2 + 0x24) * 4 * (uint)uVar1 >> 4;
      pvVar4 = CreateEventW((LPSECURITY_ATTRIBUTES)0x0,0,0,(LPCWSTR)0x0);
      *(HANDLE *)((int)this + 0x57cc) = pvVar4;
      pvVar4 = CreateThread((LPSECURITY_ATTRIBUTES)0x0,0,FUN_00426c70,DAT_005c4d88,0,
                            (LPDWORD)((int)this + 0x10));
      *(HANDLE *)((int)this + 0x14) = pvVar4;
      ppcVar5 = (char **)FUN_00459a70(*(void **)((int)this + 0xc),(int *)((int)this + 0x57c4),
                                      param_1,0x10100,DAT_00576b38,DAT_00576b3c,DAT_00576b40,
                                      DAT_00576b44,0x10,uVar6 - uVar6 % (uint)uVar1,
                                      *(char **)((int)this + 0x57cc),pcVar2);
      if ((int)ppcVar5 < 0) {
        std::_Adl_verify_range<char*,char_const*>((char **)&DAT_0056dd70,ppcVar5);
        uVar3 = 0xffffffff;
      }
      else {
        uVar3 = 0;
      }
    }
  }
  return uVar3;
}

