/* Entry: 0x00420f80; symbol: FUN_00420f80; body bytes: 189 */

void FUN_00420f80(void)

{
  MMRESULT MVar1;
  joyinfoex_tag local_3c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  _memset(&local_3c,0,0x34);
  local_3c.dwSize = 0x34;
  local_3c.dwFlags = 0xff;
  MVar1 = joyGetPosEx(0,&local_3c);
  if (MVar1 != 0) {
    MVar1 = joyGetPosEx(1,&local_3c);
    if (MVar1 != 0) {
      DAT_005c5888 = DAT_005c5888 & 0xffffefff;
      goto LAB_0042102f;
    }
  }
  joyGetDevCapsW(0,(LPJOYCAPSW)&DAT_005b9ef0,0x2d8);
  joyGetDevCapsW(1,(LPJOYCAPSW)&DAT_005ba1c8,0x2d8);
  DAT_005c5888 = DAT_005c5888 | 0x1000;
LAB_0042102f:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

