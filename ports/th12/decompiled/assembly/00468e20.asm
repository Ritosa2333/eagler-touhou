
// block 00468e20
00468e20  mov        eax, dword ptr [esp + 4]
00468e24  push       ebx
00468e25  push       esi
00468e26  mov        esi, dword ptr [edx + 4]
00468e29  mov        ebx, 1
00468e2e  mov        ecx, eax
00468e30  push       edi
00468e31  movzx      edi, word ptr [esi + 8]
00468e35  shl        ebx, cl
00468e37  test       ebx, edi
00468e39  je         0x468ea8

// block 00468e3b
00468e3b  mov        esi, dword ptr [esi + eax*4 + 0x10]
00468e3f  test       esi, esi
00468e41  jl         0x468e55

// block 00468e43
00468e43  mov        eax, dword ptr [edx + 0x100c]
00468e49  pop        edi
00468e4a  add        eax, esi
00468e4c  mov        eax, dword ptr [eax + edx + 8]
00468e50  pop        esi
00468e51  pop        ebx
00468e52  ret        4

// block 00468e55
00468e55  cmp        esi, -1
00468e58  jne        0x468e94

// block 00468e5a
00468e5a  mov        ecx, dword ptr [edx + 0x1008]
00468e60  lea        esi, [ecx - 4]
00468e63  test       esi, esi
00468e65  jl         0x468eac

// block 00468e67
00468e67  mov        dword ptr [edx + 0x1008], esi
00468e6d  mov        eax, dword ptr [esi + edx + 8]
00468e71  add        esi, -4
00468e74  mov        dword ptr [edx + 0x1008], esi
00468e7a  cmp        byte ptr [esi + edx + 8], 0x66
00468e7f  mov        dword ptr [esp + 0x10], eax
00468e83  jne        0x468eac

// block 00468e85
00468e85  fld        dword ptr [esp + 0x10]
00468e89  call       0x4931e0

// block 00468e8e
00468e8e  pop        edi
00468e8f  pop        esi
00468e90  pop        ebx
00468e91  ret        4

// block 00468e94
00468e94  mov        ecx, dword ptr [edx + 0x1014]
00468e9a  mov        edx, dword ptr [ecx]
00468e9c  mov        eax, dword ptr [edx + 4]
00468e9f  push       esi
00468ea0  call       eax

// block 00468ea2
00468ea2  pop        edi
00468ea3  pop        esi
00468ea4  pop        ebx
00468ea5  ret        4

// block 00468ea8
00468ea8  mov        eax, dword ptr [esi + eax*4 + 0x10]

// block 00468eac
00468eac  pop        edi
00468ead  pop        esi
00468eae  pop        ebx
00468eaf  ret        4
