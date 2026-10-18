/* Entry: 0x005645ea; symbol: __commit; body bytes: 125 */

/* Library Function - Single Match
    __commit
   
   Library: Visual Studio 2019 Release */

int __cdecl __commit(int _FileHandle)

{
  __acrt_ptd *p_Var1;
  int iVar2;
  uint local_14;
  int *local_10;
  int local_c;
  
  if (_FileHandle == -2) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 9;
  }
  else {
    if (((-1 < _FileHandle) && ((uint)_FileHandle < DAT_005e5890)) &&
       ((*(byte *)((&DAT_005e5690)[_FileHandle >> 6] + 0x28 + (_FileHandle & 0x3fU) * 0x38) & 1) !=
        0)) {
      local_10 = &_FileHandle;
      local_c = _FileHandle;
      local_14 = _FileHandle;
      iVar2 = FUN_00564548(&local_14,&local_10);
      return iVar2;
    }
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 9;
    FUN_005480bc();
  }
  return -1;
}

