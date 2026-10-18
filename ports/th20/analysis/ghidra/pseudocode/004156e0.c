/* Entry: 0x004156e0; symbol: FUN_004156e0; body bytes: 274 */

void __cdecl FUN_004156e0(LPCSTR param_1,int param_2)

{
  undefined4 *puVar1;
  HWND hWnd;
  HDC hdc;
  HGDIOBJ pvVar2;
  uint c;
  tagSIZE local_214;
  WCHAR local_20c [258];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  puVar1 = (undefined4 *)FUN_00414580(&DAT_005b66f0,param_2);
  pvVar2 = (HGDIOBJ)*puVar1;
  _memset(local_20c,0,0x202);
  MultiByteToWideChar(0x3a4,0,param_1,-1,local_20c,0x100);
  hWnd = (HWND)FUN_0040c300(&DAT_005b6758);
  hdc = GetDC(hWnd);
  pvVar2 = SelectObject(hdc,pvVar2);
  c = FUN_00548300((undefined1 (*) [32])local_20c);
  GetTextExtentPoint32W(hdc,local_20c,c,&local_214);
  local_214.cx = local_214.cx + 4;
  local_214.cy = local_214.cy + 4;
  SelectObject(hdc,pvVar2);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

