
// block 00430840
00430840  push       edi
00430841  xor        edi, edi
00430843  cmp        dword ptr [esi + 0x93c], 1
0043084a  jne        0x43088c

// block 0043084c
0043084c  mov        eax, dword ptr [0x4b44fc]
00430851  push       ebx
00430852  push       eax
00430853  lea        ebx, [edi + 2]
00430856  call       0x461970

// block 0043085b
0043085b  mov        ecx, dword ptr [0x4b4500]
00430861  push       ecx
00430862  call       0x461970

// block 00430867
00430867  mov        edx, dword ptr [0x4b4504]
0043086d  push       edx
0043086e  call       0x461970

// block 00430873
00430873  mov        dword ptr [0x4b44fc], edi
00430879  mov        dword ptr [0x4b4500], edi
0043087f  mov        dword ptr [0x4b4504], edi
00430885  mov        dword ptr [esi + 0x93c], ebx
0043088b  pop        ebx

// block 0043088c
0043088c  cmp        dword ptr [0x4ce8a4], edi
00430892  je         0x43089a

// block 00430894
00430894  mov        dword ptr [0x4ce8a4], edi

// block 0043089a
0043089a  pop        edi
0043089b  ret        
