/* Entry: 0x00538db0; symbol: FUN_00538db0; body bytes: 88 */

void __thiscall FUN_00538db0(void *this,LPVOID param_1,DWORD param_2)

{
  DWORD local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_c = 0;
  if (*(int *)((int)this + 8) == -0x80000000) {
    ReadFile(*(HANDLE *)((int)this + 4),param_1,param_2,&local_c,(LPOVERLAPPED)0x0);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

