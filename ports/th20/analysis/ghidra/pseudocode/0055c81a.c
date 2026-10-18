/* Entry: 0x0055c81a; symbol: tzset_from_system_nolock; body bytes: 345 */

/* Library Function - Single Match
    void __cdecl tzset_from_system_nolock(void)
   
   Library: Visual Studio 2019 Release */

void __cdecl tzset_from_system_nolock(void)

{
  long lVar1;
  long lVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  errno_t eVar5;
  DWORD DVar6;
  UINT UVar7;
  int *piVar8;
  long *plVar9;
  long local_10;
  long local_c;
  long local_8;
  
  puVar3 = (undefined4 *)FUN_0055be4a();
  puVar4 = (undefined4 *)FUN_0055be50();
  local_8 = 0;
  local_c = 0;
  local_10 = 0;
  eVar5 = FID_conflict___get_dstbias(&local_8);
  if (eVar5 == 0) {
    eVar5 = FID_conflict___get_dstbias(&local_c);
    if (eVar5 == 0) {
      eVar5 = FID_conflict___get_dstbias(&local_10);
      if (eVar5 == 0) {
        FUN_00557ba0(DAT_005e58ac);
        DAT_005e58ac = (LPVOID)0x0;
        DVar6 = GetTimeZoneInformation((LPTIME_ZONE_INFORMATION)&DAT_005e58b8);
        if (DVar6 != 0xffffffff) {
          local_8 = DAT_005e58b8 * 0x3c;
          local_c = 1;
          DAT_005e58b0 = 1;
          if (DAT_005e58fe != 0) {
            local_8 = DAT_005e590c * 0x3c + local_8;
          }
          if ((DAT_005e5952 == 0) || (DAT_005e5960 == 0)) {
            local_10 = 0;
            local_c = 0;
          }
          else {
            local_10 = (DAT_005e5960 - DAT_005e590c) * 0x3c;
          }
          _memset((void *)*puVar4,0,0x80);
          _memset((void *)puVar4[1],0,0x80);
          _memset((void *)*puVar3,0,0x40);
          _memset((void *)puVar3[1],0,0x40);
          UVar7 = ____lc_codepage_func();
          tzset_os_copy_to_tzname((wchar_t *)&DAT_005e58bc,(wchar_t *)*puVar4,(char *)*puVar3,UVar7)
          ;
          tzset_os_copy_to_tzname
                    ((wchar_t *)&DAT_005e5910,(wchar_t *)puVar4[1],(char *)puVar3[1],UVar7);
        }
        lVar1 = local_8;
        piVar8 = FUN_0055be44();
        lVar2 = local_c;
        *piVar8 = lVar1;
        plVar9 = FUN_0055be38();
        lVar1 = local_10;
        *plVar9 = lVar2;
        piVar8 = FUN_0055be3e();
        *piVar8 = lVar1;
        return;
      }
    }
  }
                    /* WARNING: Subroutine does not return */
  __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
}

