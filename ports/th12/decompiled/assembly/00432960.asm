
// block 00432960
00432960  mov        eax, dword ptr [0x4b44e8]
00432965  and        dword ptr [eax + 0x60], 0xffffffef
00432969  push       edi
0043296a  call       0x435670

// block 0043296f
0043296f  push       0
00432971  push       7
00432973  mov        edi, 0x4a104c
00432978  mov        eax, 0x4cf4e8
0043297d  call       0x454960

// block 00432982
00432982  fld        dword ptr [esi + 0x2d8]
00432988  fstp       dword ptr [0x4b2ed0]
0043298e  mov        eax, dword ptr [esi + 0x200]
00432994  mov        dword ptr [0x4cf468], eax
00432999  pop        edi
0043299a  ret        
