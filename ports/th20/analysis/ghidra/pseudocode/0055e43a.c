/* Entry: 0x0055e43a; symbol: FUN_0055e43a; body bytes: 652 */

void __cdecl FUN_0055e43a(uchar *param_1,uchar *param_2,argument_list<char> *param_3)

{
  uchar uVar1;
  bool bVar2;
  uint uVar3;
  int iVar4;
  HANDLE hFindFile;
  int iVar5;
  BOOL BVar6;
  LPCWSTR lpFileName;
  char *pcVar7;
  uchar *puVar8;
  undefined4 local_290;
  undefined4 local_28c;
  LPCWSTR local_288;
  undefined4 local_284;
  undefined4 local_280;
  char local_27c;
  undefined4 local_278;
  undefined4 local_274;
  char *local_270;
  undefined4 local_26c;
  undefined4 local_268;
  char local_264;
  argument_list<char> *local_260;
  uchar local_259;
  _WIN32_FIND_DATAW local_258;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_260 = param_3;
  if (param_2 != param_1) {
    do {
      uVar1 = *param_2;
      if (((uVar1 == '/') || (uVar1 == '\\')) || (uVar1 == ':')) break;
      param_2 = __mbsdec(param_1,param_2);
    } while (param_2 != param_1);
  }
  local_259 = *param_2;
  if ((local_259 == ':') && (param_2 != param_1 + 1)) {
    copy_and_add_argument_to_buffer<char>((char *)param_1,(char *)0x0,0,local_260);
  }
  else {
    if (((local_259 == '/') || (local_259 == '\\')) || (bVar2 = false, local_259 == ':')) {
      bVar2 = true;
    }
    local_290 = 0;
    local_28c = 0;
    local_288 = (LPCWSTR)0x0;
    local_284 = 0;
    puVar8 = (uchar *)0x0;
    if (bVar2) {
      puVar8 = param_2 + (1 - (int)param_1);
    }
    local_280 = 0;
    local_27c = '\0';
    uVar3 = __acrt_get_utf8_acp_compatibility_codepage();
    iVar4 = __acrt_mbs_to_wcs_cp<struct___crt_win32_buffer_internal_dynamic_resizing>
                      ((char *)param_1,
                       (__crt_win32_buffer<wchar_t,struct___crt_win32_buffer_internal_dynamic_resizing>
                        *)&local_290,uVar3);
    lpFileName = local_288;
    if (iVar4 != 0) {
      lpFileName = (LPCWSTR)0x0;
    }
    hFindFile = FindFirstFileExW(lpFileName,FindExInfoStandard,&local_258,FindExSearchNameMatch,
                                 (LPVOID)0x0,0);
    if (hFindFile == (HANDLE)0xffffffff) {
      copy_and_add_argument_to_buffer<char>((char *)param_1,(char *)0x0,0,local_260);
      if (local_27c != '\0') {
        FUN_00557ba0(local_288);
      }
    }
    else {
      iVar4 = *(int *)(local_260 + 4) - *(int *)local_260 >> 2;
      do {
        local_278 = 0;
        local_274 = 0;
        local_270 = (char *)0x0;
        local_26c = 0;
        local_268 = 0;
        local_264 = '\0';
        uVar3 = __acrt_get_utf8_acp_compatibility_codepage();
        iVar5 = FUN_0055e146(local_258.cFileName,
                             (__crt_win32_buffer<wchar_t,struct___crt_win32_buffer_internal_dynamic_resizing>
                              *)&local_278,&local_259,uVar3);
        pcVar7 = local_270;
        if (iVar5 != 0) {
          pcVar7 = (char *)0x0;
        }
        if (((*pcVar7 != '.') ||
            ((pcVar7[1] != '\0' && ((pcVar7[1] != '.' || (pcVar7[2] != '\0')))))) &&
           (iVar5 = copy_and_add_argument_to_buffer<char>
                              (pcVar7,(char *)param_1,(uint)puVar8,local_260), iVar5 != 0)) {
          if (local_264 != '\0') {
            FUN_00557ba0(local_270);
          }
          FindClose(hFindFile);
          if (local_27c != '\0') {
            FUN_00557ba0(local_288);
          }
          goto LAB_0055e6b8;
        }
        if (local_264 != '\0') {
          FUN_00557ba0(local_270);
        }
        BVar6 = FindNextFileW(hFindFile,&local_258);
      } while (BVar6 != 0);
      iVar5 = *(int *)(local_260 + 4) - *(int *)local_260 >> 2;
      if (iVar4 != iVar5) {
        FUN_005654b0((undefined1 *)(*(int *)local_260 + iVar4 * 4),iVar5 - iVar4,4,FUN_0055e12e);
      }
      FindClose(hFindFile);
      if (local_27c != '\0') {
        FUN_00557ba0(local_288);
      }
    }
  }
LAB_0055e6b8:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

