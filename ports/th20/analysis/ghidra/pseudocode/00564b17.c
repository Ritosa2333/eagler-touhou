/* Entry: 0x00564b17; symbol: write_text_ansi_nolock; body bytes: 219 */

/* WARNING: Function: __alloca_probe replaced with injection: alloca_probe */
/* Library Function - Single Match
    struct `anonymous namespace'::write_result __cdecl write_text_ansi_nolock(int,char const *
   const,unsigned int)
   
   Library: Visual Studio 2019 Release */

void __cdecl write_text_ansi_nolock(int param_1,char *param_2,uint param_3)

{
  char cVar1;
  char *hFile;
  BOOL BVar2;
  DWORD DVar3;
  char *pcVar4;
  char *pcVar5;
  int in_stack_00000010;
  char *local_140c;
  char local_1408 [5120];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  hFile = *(char **)((&DAT_005e5690)[(int)param_2 >> 6] + 0x18 + ((uint)param_2 & 0x3f) * 0x38);
  pcVar4 = (char *)(in_stack_00000010 + param_3);
  *(undefined4 *)param_1 = 0;
  *(undefined4 *)(param_1 + 4) = 0;
  *(undefined4 *)(param_1 + 8) = 0;
  local_140c = hFile;
  if (pcVar4 <= param_3) {
LAB_00564be1:
    FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
    return;
  }
  do {
    pcVar5 = local_1408;
    do {
      if (pcVar4 <= param_3) break;
      cVar1 = *(char *)param_3;
      param_3 = param_3 + 1;
      if (cVar1 == '\n') {
        *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + 1;
        *pcVar5 = '\r';
        pcVar5 = pcVar5 + 1;
      }
      *pcVar5 = cVar1;
      pcVar5 = pcVar5 + 1;
    } while (pcVar5 < local_1408 + 0x13ff);
    BVar2 = WriteFile(hFile,local_1408,(DWORD)(pcVar5 + -(int)local_1408),(LPDWORD)&local_140c,
                      (LPOVERLAPPED)0x0);
    if (BVar2 == 0) {
      DVar3 = GetLastError();
      *(DWORD *)param_1 = DVar3;
      goto LAB_00564be1;
    }
    *(int *)(param_1 + 4) = (int)(local_140c + *(int *)(param_1 + 4));
    if ((local_140c < pcVar5 + -(int)local_1408) || (pcVar4 <= param_3)) goto LAB_00564be1;
  } while( true );
}

