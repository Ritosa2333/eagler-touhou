
// block 004092f0
004092f0  push       ecx
004092f1  mov        dword ptr [esp], ecx
004092f4  mov        eax, dword ptr [esp]
004092f7  fld        dword ptr [esp + 8]
004092fb  fsincos    
004092fd  fmul       dword ptr [esp + 0xc]
00409301  fstp       dword ptr [eax]
00409303  fmul       dword ptr [esp + 0xc]
00409307  fstp       dword ptr [eax + 4]
0040930a  pop        ecx
0040930b  ret        8
