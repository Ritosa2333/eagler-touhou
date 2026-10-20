
// block 00410720
00410720  push       ebx
00410721  push       esi
00410722  push       edi
00410723  push       0x2c
00410725  mov        edi, edx
00410727  mov        ebx, ecx
00410729  call       0x46d04a

// block 0041072e
0041072e  push       0x2c
00410730  mov        esi, eax
00410732  push       0
00410734  push       esi
00410735  mov        dword ptr [ebx + 0x478], esi
0041073b  call       0x477420

// block 00410740
00410740  fld        dword ptr [edi]
00410742  fadd       qword ptr [0x4a3d70]
00410748  add        esp, 0x10
0041074b  fadd       qword ptr [0x4a3d28]
00410751  fstp       dword ptr [esi]
00410753  fld        dword ptr [edi + 4]
00410756  fadd       qword ptr [0x4a3d68]
0041075c  fstp       dword ptr [esi + 4]
0041075f  fld        dword ptr [edi + 8]
00410762  fstp       dword ptr [esi + 8]
00410765  mov        eax, dword ptr [esi + 0x1c]
00410768  fldz       
0041076a  test       al, 1
0041076c  jne        0x41078c

// block 0041076e
0041076e  or         eax, 1
00410771  fst        dword ptr [esi + 0x14]
00410774  mov        dword ptr [esi + 0x10], 0
0041077b  mov        dword ptr [esi + 0xc], 0xfff0bdc1
00410782  mov        dword ptr [esi + 0x18], 0x4b2ed0
00410789  mov        dword ptr [esi + 0x1c], eax

// block 0041078c
0041078c  fstp       dword ptr [esi + 0x14]
0041078f  mov        dword ptr [esi + 0x10], 0
00410796  fld        dword ptr [0x4a3db8]
0041079c  mov        dword ptr [esi + 0xc], 0xffffffff
004107a3  mov        eax, dword ptr [esi]
004107a5  fstp       dword ptr [esi + 0x20]
004107a8  mov        dword ptr [esi + 0x24], 0x40ff0000
004107af  mov        dword ptr [esi + 0x28], 0xff00ff
004107b6  mov        dword ptr [ebx + 0x430], eax
004107bc  mov        ecx, dword ptr [esi + 4]
004107bf  mov        dword ptr [ebx + 0x434], ecx
004107c5  mov        edx, dword ptr [esi + 8]
004107c8  pop        edi
004107c9  pop        esi
004107ca  mov        dword ptr [ebx + 0x438], edx
004107d0  mov        dword ptr [ebx + 0x20], 0xd
004107d7  xor        eax, eax
004107d9  pop        ebx
004107da  ret        
