
// block 00427d00
00427d00  push       ecx
00427d01  mov        dword ptr [esp], ecx
00427d04  mov        eax, dword ptr [esp]
00427d07  fld        dword ptr [esp + 8]
00427d0b  fsincos    
00427d0d  fmul       dword ptr [esp + 0xc]
00427d11  fstp       dword ptr [eax]
00427d13  fmul       dword ptr [esp + 0xc]
00427d17  fstp       dword ptr [eax + 4]
00427d1a  pop        ecx
00427d1b  ret        8
