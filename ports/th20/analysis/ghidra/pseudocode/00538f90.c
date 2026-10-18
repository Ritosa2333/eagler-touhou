/* Entry: 0x00538f90; symbol: FUN_00538f90; body bytes: 106 */

void __thiscall FUN_00538f90(void *this,LPCVOID param_1,DWORD param_2)

{
  DWORD local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_c = 0;
  if (*(int *)((int)this + 8) == 0x40000000) {
    WriteFile(*(HANDLE *)((int)this + 4),param_1,param_2,&local_c,(LPOVERLAPPED)0x0);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

