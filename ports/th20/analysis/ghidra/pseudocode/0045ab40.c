/* Entry: 0x0045ab40; symbol: FUN_0045ab40; body bytes: 327 */

void __thiscall FUN_0045ab40(void *this,char **param_1,char *param_2,char *param_3)

{
  char *pcVar1;
  WCHAR local_214 [262];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *(char **)((int)this + 0x78) = param_3;
  *(undefined4 *)((int)this + 0x7c) = 0;
  if ((*(int *)((int)this + 0x78) == 1) && (param_1 != (char **)0x0)) {
    std::_Adl_verify_range<char*,char_const*>((char **)"Streaming File Open %s\r\n",param_1);
    _memset(local_214,0,0x20a);
    MultiByteToWideChar(0x3a4,0,(LPCSTR)param_1,-1,local_214,0x104);
    pcVar1 = (char *)CreateFileW(local_214,0x80000000,1,(LPSECURITY_ATTRIBUTES)0x0,3,0x8000080,
                                 (HANDLE)0x0);
    *(char **)((int)this + 0x8c) = pcVar1;
    if (*(char **)((int)this + 0x8c) == (char *)0xffffffff) {
      std::_Adl_verify_range<char*,char_const*>((char **)"error!\n",(char **)this);
    }
    else {
      *(char **)((int)this + 0x90) = param_2;
      *(char ***)((int)this + 0x94) = param_1;
      FUN_0045b5f0(this,'\0',0);
      *(char **)((int)this + 0x2c) = *(char **)((int)this + 8);
      *(char **)((int)this + 0x9c) = (char *)0x0;
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

