
// block 0048c9bf
0048c9bf  mov        edi, edi
0048c9c1  push       ebp
0048c9c2  mov        ebp, esp
0048c9c4  mov        eax, dword ptr [ebp + 8]
0048c9c7  push       esi
0048c9c8  push       edi
0048c9c9  test       eax, eax
0048c9cb  jl         0x48ca26

// block 0048c9cd
0048c9cd  cmp        eax, dword ptr [0x4d6308]
0048c9d3  jae        0x48ca26

// block 0048c9d5
0048c9d5  mov        ecx, eax
0048c9d7  sar        ecx, 5
0048c9da  mov        esi, eax
0048c9dc  and        esi, 0x1f
0048c9df  lea        edi, [ecx*4 + 0x4d6320]
0048c9e6  mov        ecx, dword ptr [edi]
0048c9e8  shl        esi, 6
0048c9eb  cmp        dword ptr [esi + ecx], -1
0048c9ef  jne        0x48ca26

// block 0048c9f1
0048c9f1  cmp        dword ptr [0x4ad134], 1
0048c9f8  push       ebx
0048c9f9  mov        ebx, dword ptr [ebp + 0xc]
0048c9fc  jne        0x48ca1c

// block 0048c9fe
0048c9fe  sub        eax, 0
0048ca01  je         0x48ca13

// block 0048ca03
0048ca03  dec        eax
0048ca04  je         0x48ca0e

// block 0048ca06
0048ca06  dec        eax
0048ca07  jne        0x48ca1c

// block 0048ca09
0048ca09  push       ebx
0048ca0a  push       -0xc
0048ca0c  jmp        0x48ca16

// block 0048ca0e
0048ca0e  push       ebx
0048ca0f  push       -0xb
0048ca11  jmp        0x48ca16

// block 0048ca13
0048ca13  push       ebx
0048ca14  push       -0xa

// block 0048ca16
0048ca16  call       dword ptr [0x4981a4]

// block 0048ca1c
0048ca1c  mov        eax, dword ptr [edi]
0048ca1e  mov        dword ptr [esi + eax], ebx
0048ca21  xor        eax, eax
0048ca23  pop        ebx
0048ca24  jmp        0x48ca3c

// block 0048ca26
0048ca26  call       0x46e96f

// block 0048ca2b
0048ca2b  mov        dword ptr [eax], 9
0048ca31  call       0x46e982

// block 0048ca36
0048ca36  and        dword ptr [eax], 0
0048ca39  or         eax, 0xffffffff

// block 0048ca3c
0048ca3c  pop        edi
0048ca3d  pop        esi
0048ca3e  pop        ebp
0048ca3f  ret        
