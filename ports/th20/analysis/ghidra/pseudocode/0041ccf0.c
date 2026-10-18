/* Entry: 0x0041ccf0; symbol: FUN_0041ccf0; body bytes: 905 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_0041ccf0(void *this,HINSTANCE param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  int iVar2;
  HWND pHVar3;
  uint local_50;
  LPCWSTR local_4c;
  WNDCLASSW local_40;
  tagRECT local_18;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  bVar1 = FUN_0041d0c0();
  if (CONCAT31(extraout_var,bVar1) == 0) {
    local_4c = L"TH20 - Fossilized Wonder. ver 1.00c";
  }
  else {
    local_4c = L"東方錦上京\x3000～ Fossilized Wonder. ver 1.00c";
  }
  _memset(&local_40,0,0x28);
  local_40.hbrBackground = (HBRUSH)GetStockObject(4);
  local_40.hCursor = LoadCursorW((HINSTANCE)0x0,(LPCWSTR)0x7f00);
  local_40.hInstance = param_1;
  local_40.lpfnWndProc = FUN_0041d350;
  *(undefined1 *)((int)this + 0x68) = 1;
  *(undefined1 *)((int)this + 0x69) = 0;
  local_40.lpszClassName = L"BASE";
  RegisterClassW(&local_40);
  FUN_0041e030(this,DAT_005c4f80);
  iVar2 = FUN_0041cc70((int)this);
  local_50 = (uint)(2 < iVar2);
  _DAT_005c4e44 = local_50;
  if ((DAT_005c4f84 == '\0') && (DAT_005c4f89 == '\x02')) {
    *(uint *)((int)this + 0x2090) = *(uint *)((int)this + 0x2090) | 4;
  }
  else {
    *(uint *)((int)this + 0x2090) = *(uint *)((int)this + 0x2090) & 0xfffffffb;
  }
  FUN_0041cc90((void *)((int)this + 0x2104),0xf);
  FUN_0041cc90((void *)((int)this + 0x2110),0xc);
  FUN_0041cc90((void *)((int)this + 0x211c),0xc);
  FUN_0041cc90((void *)((int)this + 0x2128),8);
  *(undefined4 *)((int)this + 0x2100) = 0;
  FUN_0041e050(this,1);
  iVar2 = FUN_00415800(0x5c4d40);
  if (iVar2 == 0) {
    pHVar3 = CreateWindowExW(0,L"BASE",local_4c,0x90000000,0,0,*(int *)((int)this + 0x20a8),
                             *(int *)((int)this + 0x20ac),(HWND)0x0,(HMENU)0x0,param_1,(LPVOID)0x0);
    FUN_004117a0(this,pHVar3);
  }
  else {
    iVar2 = FUN_0041cc70(0x5b6758);
    if ((iVar2 == 8) || (iVar2 = FUN_0041cc70(0x5b6758), iVar2 == 9)) {
      pHVar3 = CreateWindowExW(0x40000,L"BASE",local_4c,0x80000000,0,0,*(int *)((int)this + 0x20a8),
                               *(int *)((int)this + 0x20ac),(HWND)0x0,(HMENU)0x0,param_1,(LPVOID)0x0
                              );
      *(HWND *)this = pHVar3;
                    /* WARNING: Load size is inaccurate */
      SetWindowLongW(*this,-0x10,0);
                    /* WARNING: Load size is inaccurate */
      ShowWindow(*this,5);
                    /* WARNING: Load size is inaccurate */
      InvalidateRect(*this,(RECT *)0x0,1);
                    /* WARNING: Load size is inaccurate */
      UpdateWindow(*this);
    }
    else {
      local_18.left = 0;
      local_18.top = 0;
      local_18.right = *(int *)((int)this + 0x20a8);
      local_18.bottom = *(int *)((int)this + 0x20ac);
      AdjustWindowRectEx(&local_18,0x10cb0000,0,0);
      pHVar3 = CreateWindowExW(0,L"BASE",local_4c,0x10cb0000,DAT_005c4f90,DAT_005c4f94,
                               local_18.right - local_18.left,local_18.bottom - local_18.top,
                               (HWND)0x0,(HMENU)0x0,param_1,(LPVOID)0x0);
      *(HWND *)this = pHVar3;
                    /* WARNING: Load size is inaccurate */
      ShowWindow(*this,5);
                    /* WARNING: Load size is inaccurate */
      InvalidateRect(*this,(RECT *)0x0,1);
                    /* WARNING: Load size is inaccurate */
      UpdateWindow(*this);
    }
  }
                    /* WARNING: Load size is inaccurate */
  GetWindowRect(*this,(LPRECT)&DAT_005c4d4c);
                    /* WARNING: Load size is inaccurate */
  DAT_005c4d88 = *this;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

