/* Entry: 0x00564bf2; symbol: write_text_utf16le_nolock; body bytes: 233 */

/* WARNING: Function: __alloca_probe replaced with injection: alloca_probe */
/* Library Function - Single Match
    struct `anonymous namespace'::write_result __cdecl write_text_utf16le_nolock(int,char const *
   const,unsigned int)
   
   Library: Visual Studio 2019 Release */

void __cdecl write_text_utf16le_nolock(int param_1,char *param_2,uint param_3)

{
  short sVar1;
  BOOL BVar2;
  DWORD DVar3;
  short *psVar4;
  short *psVar5;
  int in_stack_00000010;
  uint local_1410;
  HANDLE local_140c;
  short local_1408 [2560];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_140c = *(HANDLE *)
                ((&DAT_005e5690)[(int)param_2 >> 6] + 0x18 + ((uint)param_2 & 0x3f) * 0x38);
  psVar4 = (short *)(in_stack_00000010 + param_3);
  *(undefined4 *)param_1 = 0;
  *(undefined4 *)(param_1 + 4) = 0;
  *(undefined4 *)(param_1 + 8) = 0;
  do {
    if (psVar4 <= param_3) break;
    psVar5 = local_1408;
    do {
      if (psVar4 <= param_3) break;
      sVar1 = *(short *)param_3;
      param_3 = param_3 + 2;
      if (sVar1 == 10) {
        *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + 2;
        *psVar5 = 0xd;
        psVar5 = psVar5 + 1;
      }
      *psVar5 = sVar1;
      psVar5 = psVar5 + 1;
    } while (psVar5 < local_1408 + 0x9ff);
    BVar2 = WriteFile(local_140c,local_1408,(int)psVar5 - (int)local_1408,&local_1410,
                      (LPOVERLAPPED)0x0);
    if (BVar2 == 0) {
      DVar3 = GetLastError();
      *(DWORD *)param_1 = DVar3;
      break;
    }
    *(int *)(param_1 + 4) = *(int *)(param_1 + 4) + local_1410;
  } while ((uint)((int)psVar5 - (int)local_1408) <= local_1410);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

