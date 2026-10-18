/* Entry: 0x00546495; symbol: _FilterSetCurrentException; body bytes: 88 */

/* Library Function - Single Match
    unsigned long __cdecl _FilterSetCurrentException(struct _EXCEPTION_POINTERS *,unsigned char)
   
   Library: Visual Studio 2019 Release */

ulong __cdecl _FilterSetCurrentException(_EXCEPTION_POINTERS *param_1,uchar param_2)

{
  PEXCEPTION_RECORD pEVar1;
  PCONTEXT pCVar2;
  code *pcVar3;
  int iVar4;
  ulong uVar5;
  
  if ((((param_2 != '\0') &&
       (pEVar1 = param_1->ExceptionRecord, pEVar1->ExceptionCode == 0xe06d7363)) &&
      (pEVar1->NumberParameters == 3)) &&
     (((pEVar1->ExceptionInformation[0] == 0x19930520 ||
       (pEVar1->ExceptionInformation[0] == 0x19930521)) ||
      (pEVar1->ExceptionInformation[0] == 0x19930522)))) {
    iVar4 = ___vcrt_getptd();
    *(PEXCEPTION_RECORD *)(iVar4 + 0x10) = pEVar1;
    pCVar2 = param_1->ContextRecord;
    iVar4 = ___vcrt_getptd();
    *(PCONTEXT *)(iVar4 + 0x14) = pCVar2;
    terminate();
    pcVar3 = (code *)swi(3);
    uVar5 = (*pcVar3)();
    return uVar5;
  }
  return 0;
}

