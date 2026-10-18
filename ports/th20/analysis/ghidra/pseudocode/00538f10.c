/* Entry: 0x00538f10; symbol: FUN_00538f10; body bytes: 51 */

bool __thiscall FUN_00538f10(void *this,LONG param_1,DWORD param_2)

{
  bool bVar1;
  
  bVar1 = *(int *)((int)this + 4) != -1;
  if (bVar1) {
    SetFilePointer(*(HANDLE *)((int)this + 4),param_1,(PLONG)0x0,param_2);
  }
  return bVar1;
}

