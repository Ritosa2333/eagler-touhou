
// block 00432850
00432850  push       ecx
00432851  fldz       
00432853  push       ebx
00432854  push       ebp
00432855  push       esi
00432856  mov        esi, dword ptr [0x4b4510]
0043285c  mov        dword ptr [esi + 4], 2
00432863  mov        eax, dword ptr [esi + 0x20]
00432866  xor        ebp, ebp
00432868  push       edi
00432869  mov        edi, 0xfff0bdc1
0043286e  mov        edx, 0x4b2ed0
00432873  test       al, 1
00432875  jne        0x432889

// block 00432877
00432877  or         eax, 1
0043287a  fst        dword ptr [esi + 0x18]
0043287d  mov        dword ptr [esi + 0x14], ebp
00432880  mov        dword ptr [esi + 0x10], edi
00432883  mov        dword ptr [esi + 0x1c], edx
00432886  mov        dword ptr [esi + 0x20], eax

// block 00432889
00432889  or         ecx, 0xffffffff
0043288c  fst        dword ptr [esi + 0x18]
0043288f  mov        dword ptr [esi + 0x14], ebp
00432892  mov        dword ptr [esi + 0x10], ecx
00432895  mov        eax, dword ptr [esi + 0x34]
00432898  test       al, 1
0043289a  jne        0x4328ae

// block 0043289c
0043289c  or         eax, 1
0043289f  fst        dword ptr [esi + 0x2c]
004328a2  mov        dword ptr [esi + 0x28], ebp
004328a5  mov        dword ptr [esi + 0x24], edi
004328a8  mov        dword ptr [esi + 0x30], edx
004328ab  mov        dword ptr [esi + 0x34], eax

// block 004328ae
004328ae  mov        eax, dword ptr [0x4b44e8]
004328b3  fstp       dword ptr [esi + 0x2c]
004328b6  mov        dword ptr [esi + 0x28], ebp
004328b9  mov        dword ptr [esi + 0x24], ecx
004328bc  or         dword ptr [eax + 0x60], 0x10
004328c0  mov        edi, dword ptr [0x4cee70]
004328c6  push       0x4b
004328c8  lea        eax, [esp + 0x14]
004328cc  push       eax
004328cd  call       0x4357c0

// block 004328d2
004328d2  mov        eax, dword ptr [eax]
004328d4  push       eax
004328d5  mov        dword ptr [esi + 0x1ec], eax
004328db  call       0x435750

// block 004328e0
004328e0  mov        ecx, dword ptr [0x4b43e4]
004328e6  mov        edi, dword ptr [ecx + 0x6d44]
004328ec  push       0x68
004328ee  lea        edx, [esp + 0x14]
004328f2  push       edx
004328f3  mov        dword ptr [esi + 0x2dc], edi
004328f9  call       0x4357c0

// block 004328fe
004328fe  mov        eax, dword ptr [eax]
00432900  push       eax
00432901  mov        ebx, 3
00432906  mov        dword ptr [esi + 0x1e8], eax
0043290c  call       0x461970

// block 00432911
00432911  call       0x423020

// block 00432916
00432916  push       ebp
00432917  push       6
00432919  mov        edi, 0x4a1044
0043291e  mov        eax, 0x4cf4e8
00432923  call       0x454960

// block 00432928
00432928  fld        dword ptr [0x4b2ed0]
0043292e  fstp       dword ptr [esi + 0x2d8]
00432934  mov        eax, dword ptr [0x4cf468]
00432939  fld1       
0043293b  pop        edi
0043293c  fstp       dword ptr [0x4b2ed0]
00432942  mov        dword ptr [esi + 0x200], eax
00432948  pop        esi
00432949  mov        dword ptr [0x4cf468], ebp
0043294f  pop        ebp
00432950  pop        ebx
00432951  pop        ecx
00432952  ret        
