
// block 00464d50
00464d50  push       ecx
00464d51  test       byte ptr [esi + 0x30], 1
00464d55  jne        0x464d76

// block 00464d57
00464d57  fld        dword ptr [esi + 0x18]
00464d5a  sub        esp, 8
00464d5d  fstp       dword ptr [esp + 4]
00464d61  lea        ecx, [esi + 0xc]
00464d64  fld        dword ptr [esi + 0x1c]
00464d67  fstp       dword ptr [esp]
00464d6a  call       0x465390

// block 00464d6f
00464d6f  fldz       
00464d71  fstp       dword ptr [esi + 0x14]
00464d74  pop        ecx
00464d75  ret        

// block 00464d76
00464d76  fld        dword ptr [esi + 0x24]
00464d79  push       ecx
00464d7a  fadd       dword ptr [esi + 0x20]
00464d7d  fstp       dword ptr [esi + 0x20]
00464d80  fld        dword ptr [esi + 0x18]
00464d83  fadd       dword ptr [esi + 0x1c]
00464d86  fstp       dword ptr [esp + 4]
00464d8a  fld        dword ptr [esp + 4]
00464d8e  fstp       dword ptr [esp]
00464d91  call       0x4646e0

// block 00464d96
00464d96  push       ecx
00464d97  fstp       dword ptr [esp]
00464d9a  call       0x4646e0

// block 00464d9f
00464d9f  fstp       dword ptr [esi + 0x1c]
00464da2  pop        ecx
00464da3  ret        
