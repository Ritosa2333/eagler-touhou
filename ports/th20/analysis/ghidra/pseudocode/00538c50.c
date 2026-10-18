/* Entry: 0x00538c50; symbol: FUN_00538c50; body bytes: 346 */

void __thiscall FUN_00538c50(void *this,LPCSTR param_1,char *param_2)

{
  bool bVar1;
  HANDLE pvVar2;
  DWORD local_21c;
  char *local_218;
  WCHAR local_210 [260];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  bVar1 = false;
  local_21c = 0;
                    /* WARNING: Load size is inaccurate */
  (**(code **)(*this + 4))();
  local_218 = param_2;
  do {
    if (*local_218 == '\0') {
LAB_00538d12:
      if (*local_218 != '\0') {
        FUN_00538e10((undefined1 (*) [16])local_210,param_1);
        pvVar2 = CreateFileW(local_210,*(DWORD *)((int)this + 8),1,(LPSECURITY_ATTRIBUTES)0x0,
                             local_21c,0x8000080,(HANDLE)0x0);
        *(HANDLE *)((int)this + 4) = pvVar2;
        if ((*(int *)((int)this + 4) != -1) && (bVar1)) {
          SetFilePointer(*(HANDLE *)((int)this + 4),0,(PLONG)0x0,2);
        }
      }
      FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
      return;
    }
    if (*local_218 == 'r') {
      *(undefined4 *)((int)this + 8) = 0x80000000;
      local_21c = 3;
      goto LAB_00538d12;
    }
    if (*local_218 == 'a') {
      bVar1 = true;
      *(undefined4 *)((int)this + 8) = 0x40000000;
      local_21c = 4;
      goto LAB_00538d12;
    }
    local_218 = local_218 + 1;
  } while( true );
}

