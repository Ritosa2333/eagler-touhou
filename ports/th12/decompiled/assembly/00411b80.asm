
// block 00411b80
00411b80  sub        esp, 0x10
00411b83  fld        dword ptr [esp + 0x14]
00411b87  push       esi
00411b88  mov        esi, dword ptr [0x4b43b8]
00411b8e  fstp       dword ptr [esp + 8]
00411b92  cmp        dword ptr [esi + 0x18fbc], 0
00411b99  fld        dword ptr [esp + 0x1c]
00411b9d  fstp       dword ptr [esp + 0xc]
00411ba1  fldz       
00411ba3  fstp       dword ptr [esp + 0x10]
00411ba7  jne        0x411bd1

// block 00411ba9
00411ba9  mov        ecx, dword ptr [esi + 0x18fb4]
00411baf  push       0
00411bb1  push       0x12
00411bb3  lea        eax, [esp + 0x20]
00411bb7  push       eax
00411bb8  push       ecx
00411bb9  mov        eax, 0x17
00411bbe  lea        ecx, [esp + 0x18]
00411bc2  call       0x4615a0

// block 00411bc7
00411bc7  mov        edx, dword ptr [esp + 0x18]
00411bcb  mov        dword ptr [esi + 0x18fbc], edx

// block 00411bd1
00411bd1  pop        esi
00411bd2  add        esp, 0x10
00411bd5  ret        8
