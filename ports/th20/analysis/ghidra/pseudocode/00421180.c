/* Entry: 0x00421180; symbol: FUN_00421180; body bytes: 1239 */

/* WARNING: Type propagation algorithm not settling */

void __cdecl FUN_00421180(uint *param_1)

{
  HRESULT HVar1;
  HRESULT HVar2;
  int iVar3;
  wchar_t *pwVar4;
  IID *local_9c;
  BSTR local_98;
  BSTR local_94;
  uint local_84;
  _union_2683 local_80;
  int *local_70;
  int *local_6c;
  IUnknown *local_68;
  int local_64;
  uint local_60;
  uint local_5c [2];
  undefined1 local_54 [76];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_70 = (int *)0x0;
  local_6c = (int *)0x0;
  local_5c[1] = 0;
  _memset(local_54,0,0x4c);
  local_68 = (IUnknown *)0x0;
  local_94 = (BSTR)0x0;
  local_98 = (BSTR)0x0;
  local_9c = (IID *)0x0;
  local_5c[0] = 0;
  VariantInit((VARIANTARG *)&local_80.n2);
  HVar1 = CoInitialize((LPVOID)0x0);
  HVar2 = CoCreateInstance((IID *)&DAT_0056d4f4,(LPUNKNOWN)0x0,1,(IID *)&DAT_0056d504,&local_70);
  if (((((-1 < HVar2) && (local_70 != (int *)0x0)) &&
       (local_94 = SysAllocString(L"\\\\.\\root\\cimv2"), local_94 != (BSTR)0x0)) &&
      ((local_9c = (IID *)SysAllocString(L"Win32_PNPEntity"), local_9c != (IID *)0x0 &&
       (local_98 = SysAllocString(L"DeviceID"), local_98 != (BSTR)0x0)))) &&
     ((iVar3 = (**(code **)(*local_70 + 0xc))(local_70,local_94,0,0,0,0,0,0,&local_68), -1 < iVar3
      && (local_68 != (IUnknown *)0x0)))) {
    CoSetProxyBlanket(local_68,10,0,(OLECHAR *)0x0,3,3,(RPC_AUTH_IDENTITY_HANDLE)0x0,0);
    HVar2 = (*local_68->lpVtbl[6].QueryInterface)(local_68,local_9c,(void **)0x0);
    if ((-1 < HVar2) && (local_6c != (int *)0x0)) {
      while ((iVar3 = (**(code **)(*local_6c + 0x10))(local_6c,10000,0x14,local_5c + 1,local_5c),
             -1 < iVar3 && (local_5c[0] != 0))) {
        for (local_84 = 0; local_84 < local_5c[0]; local_84 = local_84 + 1) {
          if (*(int *)(local_54 + local_84 * 4 + -4) != 0) {
            iVar3 = (**(code **)(**(int **)(local_54 + local_84 * 4 + -4) + 0x10))
                              (*(undefined4 *)(local_54 + local_84 * 4 + -4),local_98,0,&local_80,0,
                               0);
            if ((((-1 < iVar3) && (local_80.n2.vt == 8)) &&
                ((undefined1 (*) [16])local_80._8_4_ != (undefined1 (*) [16])0x0)) &&
               (iVar3 = FUN_00422b60((undefined1 (*) [16])local_80._8_4_,
                                     (undefined1 (*) [16])&DAT_0056d568), iVar3 != 0)) {
              local_64 = 0;
              local_60 = 0;
              pwVar4 = (wchar_t *)
                       FUN_00422b60((undefined1 (*) [16])local_80._8_4_,(undefined1 (*) [16])L"VID_"
                                   );
              if ((pwVar4 != (wchar_t *)0x0) &&
                 (iVar3 = FUN_00422bc0(pwVar4,L"VID_%4X"), iVar3 != 1)) {
                local_60 = 0;
              }
              pwVar4 = (wchar_t *)
                       FUN_00422b60((undefined1 (*) [16])local_80._8_4_,(undefined1 (*) [16])L"PID_"
                                   );
              if ((pwVar4 != (wchar_t *)0x0) &&
                 (iVar3 = FUN_00422bc0(pwVar4,L"PID_%4X"), iVar3 != 1)) {
                local_64 = 0;
              }
              if ((local_60 & 0xffff | local_64 << 0x10) == *param_1) goto LAB_0042157a;
            }
            if (*(int *)(local_54 + local_84 * 4 + -4) != 0) {
              (**(code **)(**(int **)(local_54 + local_84 * 4 + -4) + 8))
                        (*(undefined4 *)(local_54 + local_84 * 4 + -4));
              *(undefined4 *)(local_54 + local_84 * 4 + -4) = 0;
            }
          }
        }
      }
    }
  }
LAB_0042157a:
  if (local_94 != (BSTR)0x0) {
    SysFreeString(local_94);
  }
  if (local_98 != (BSTR)0x0) {
    SysFreeString(local_98);
  }
  if (local_9c != (IID *)0x0) {
    SysFreeString((BSTR)local_9c);
  }
  for (local_84 = 0; local_84 < 0x14; local_84 = local_84 + 1) {
    if (*(int *)(local_54 + local_84 * 4 + -4) != 0) {
      (**(code **)(**(int **)(local_54 + local_84 * 4 + -4) + 8))
                (*(undefined4 *)(local_54 + local_84 * 4 + -4));
      *(undefined4 *)(local_54 + local_84 * 4 + -4) = 0;
    }
  }
  if (local_6c != (int *)0x0) {
    (**(code **)(*local_6c + 8))(local_6c);
    local_6c = (int *)0x0;
  }
  if (local_70 != (int *)0x0) {
    (**(code **)(*local_70 + 8))(local_70);
    local_70 = (int *)0x0;
  }
  if (local_68 != (IUnknown *)0x0) {
    (*local_68->lpVtbl->Release)(local_68);
    local_68 = (IUnknown *)0x0;
  }
  if (-1 < HVar1) {
    CoUninitialize();
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

