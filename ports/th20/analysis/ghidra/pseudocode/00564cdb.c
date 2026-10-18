/* Entry: 0x00564cdb; symbol: write_text_utf8_nolock; body bytes: 306 */

/* WARNING: Function: __alloca_probe replaced with injection: alloca_probe */
/* Library Function - Single Match
    struct `anonymous namespace'::write_result __cdecl write_text_utf8_nolock(int,char const *
   const,unsigned int)
   
   Library: Visual Studio 2019 Release */

void __cdecl write_text_utf8_nolock(int param_1,char *param_2,uint param_3)

{
  WCHAR WVar1;
  WCHAR *pWVar2;
  uint uVar3;
  BOOL BVar4;
  DWORD DVar5;
  uint uVar6;
  WCHAR *pWVar7;
  int in_stack_00000010;
  DWORD local_1418;
  HANDLE local_1414;
  WCHAR *local_1410;
  CHAR local_140c [3416];
  WCHAR local_6b4 [854];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_1414 = *(HANDLE *)
                ((&DAT_005e5690)[(int)param_2 >> 6] + 0x18 + ((uint)param_2 & 0x3f) * 0x38);
  local_1410 = (WCHAR *)(in_stack_00000010 + param_3);
  *(undefined4 *)param_1 = 0;
  *(undefined4 *)(param_1 + 4) = 0;
  *(undefined4 *)(param_1 + 8) = 0;
  pWVar7 = (WCHAR *)param_3;
  if (param_3 < local_1410) {
    do {
      pWVar2 = local_6b4;
      do {
        if (local_1410 <= pWVar7) break;
        WVar1 = *pWVar7;
        pWVar7 = pWVar7 + 1;
        if (WVar1 == L'\n') {
          *pWVar2 = L'\r';
          pWVar2 = pWVar2 + 1;
        }
        *pWVar2 = WVar1;
        pWVar2 = pWVar2 + 1;
      } while (pWVar2 < local_6b4 + 0x354);
      uVar3 = FUN_0055de4b(0xfde9,0,local_6b4,(int)pWVar2 - (int)local_6b4 >> 1,local_140c,0xd55,
                           (LPCSTR)0x0,(LPBOOL)0x0);
      if (uVar3 == 0) {
LAB_00564df4:
        DVar5 = GetLastError();
        *(DWORD *)param_1 = DVar5;
        break;
      }
      uVar6 = 0;
      if (uVar3 != 0) {
        do {
          BVar4 = WriteFile(local_1414,local_140c + uVar6,uVar3 - uVar6,&local_1418,
                            (LPOVERLAPPED)0x0);
          if (BVar4 == 0) goto LAB_00564df4;
          uVar6 = uVar6 + local_1418;
        } while (uVar6 < uVar3);
      }
      *(uint *)(param_1 + 4) = (int)pWVar7 - param_3;
    } while (pWVar7 < local_1410);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

