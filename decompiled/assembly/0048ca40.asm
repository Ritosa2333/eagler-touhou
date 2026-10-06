
// block 0048ca40
0048ca40  mov        edi, edi
0048ca42  push       ebp
0048ca43  mov        ebp, esp
0048ca45  mov        ecx, dword ptr [ebp + 8]
0048ca48  push       ebx
0048ca49  xor        ebx, ebx
0048ca4b  cmp        ecx, ebx
0048ca4d  push       esi
0048ca4e  push       edi
0048ca4f  jl         0x48caac

// block 0048ca51
0048ca51  cmp        ecx, dword ptr [0x4d6308]
0048ca57  jae        0x48caac

// block 0048ca59
0048ca59  mov        eax, ecx
0048ca5b  sar        eax, 5
0048ca5e  mov        esi, ecx
0048ca60  lea        edi, [eax*4 + 0x4d6320]
0048ca67  mov        eax, dword ptr [edi]
0048ca69  and        esi, 0x1f
0048ca6c  shl        esi, 6
0048ca6f  add        eax, esi
0048ca71  test       byte ptr [eax + 4], 1
0048ca75  je         0x48caac

// block 0048ca77
0048ca77  cmp        dword ptr [eax], -1
0048ca7a  je         0x48caac

// block 0048ca7c
0048ca7c  cmp        dword ptr [0x4ad134], 1
0048ca83  jne        0x48caa2

// block 0048ca85
0048ca85  sub        ecx, ebx
0048ca87  je         0x48ca99

// block 0048ca89
0048ca89  dec        ecx
0048ca8a  je         0x48ca94

// block 0048ca8c
0048ca8c  dec        ecx
0048ca8d  jne        0x48caa2

// block 0048ca8f
0048ca8f  push       ebx
0048ca90  push       -0xc
0048ca92  jmp        0x48ca9c

// block 0048ca94
0048ca94  push       ebx
0048ca95  push       -0xb
0048ca97  jmp        0x48ca9c

// block 0048ca99
0048ca99  push       ebx
0048ca9a  push       -0xa

// block 0048ca9c
0048ca9c  call       dword ptr [0x4981a4]

// block 0048caa2
0048caa2  mov        eax, dword ptr [edi]
0048caa4  or         dword ptr [esi + eax], 0xffffffff
0048caa8  xor        eax, eax
0048caaa  jmp        0x48cac1

// block 0048caac
0048caac  call       0x46e96f

// block 0048cab1
0048cab1  mov        dword ptr [eax], 9
0048cab7  call       0x46e982

// block 0048cabc
0048cabc  mov        dword ptr [eax], ebx
0048cabe  or         eax, 0xffffffff

// block 0048cac1
0048cac1  pop        edi
0048cac2  pop        esi
0048cac3  pop        ebx
0048cac4  pop        ebp
0048cac5  ret        
