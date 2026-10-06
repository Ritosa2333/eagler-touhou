
// block 0048c004
0048c004  mov        edi, edi
0048c006  push       ebp
0048c007  mov        ebp, esp
0048c009  push       esi
0048c00a  mov        esi, dword ptr [ebp + 0xc]
0048c00d  test       byte ptr [esi + 0xc], 0x40
0048c011  push       edi
0048c012  jne        0x48c08d

// block 0048c014
0048c014  push       esi
0048c015  call       0x47c1da

// block 0048c01a
0048c01a  pop        ecx
0048c01b  mov        edx, 0x4adbb0
0048c020  cmp        eax, -1
0048c023  je         0x48c040

// block 0048c025
0048c025  cmp        eax, -2
0048c028  je         0x48c040

// block 0048c02a
0048c02a  mov        ecx, eax
0048c02c  and        ecx, 0x1f
0048c02f  mov        edi, eax
0048c031  sar        edi, 5
0048c034  shl        ecx, 6
0048c037  add        ecx, dword ptr [edi*4 + 0x4d6320]
0048c03e  jmp        0x48c042

// block 0048c040
0048c040  mov        ecx, edx

// block 0048c042
0048c042  test       byte ptr [ecx + 0x24], 0x7f
0048c046  jne        0x48c06e

// block 0048c048
0048c048  cmp        eax, -1
0048c04b  je         0x48c066

// block 0048c04d
0048c04d  cmp        eax, -2
0048c050  je         0x48c066

// block 0048c052
0048c052  mov        ecx, eax
0048c054  and        eax, 0x1f
0048c057  sar        ecx, 5
0048c05a  shl        eax, 6
0048c05d  add        eax, dword ptr [ecx*4 + 0x4d6320]
0048c064  jmp        0x48c068

// block 0048c066
0048c066  mov        eax, edx

// block 0048c068
0048c068  test       byte ptr [eax + 0x24], 0x80
0048c06c  je         0x48c08d

// block 0048c06e
0048c06e  call       0x46e96f

// block 0048c073
0048c073  xor        edi, edi
0048c075  push       edi
0048c076  push       edi
0048c077  push       edi
0048c078  push       edi
0048c079  push       edi
0048c07a  mov        dword ptr [eax], 0x16
0048c080  call       0x470f2d

// block 0048c085
0048c085  add        esp, 0x14
0048c088  or         eax, 0xffffffff
0048c08b  jmp        0x48c0d7

// block 0048c08d
0048c08d  push       ebx
0048c08e  mov        ebx, dword ptr [ebp + 8]
0048c091  cmp        ebx, -1
0048c094  je         0x48c0d3

// block 0048c096
0048c096  mov        eax, dword ptr [esi + 0xc]
0048c099  test       al, 1
0048c09b  jne        0x48c0a5

// block 0048c09d
0048c09d  test       al, al
0048c09f  jns        0x48c0d3

// block 0048c0a1
0048c0a1  test       al, 2
0048c0a3  jne        0x48c0d3

// block 0048c0a5
0048c0a5  xor        edi, edi
0048c0a7  cmp        dword ptr [esi + 8], edi
0048c0aa  jne        0x48c0b3

// block 0048c0ac
0048c0ac  push       esi
0048c0ad  call       0x47bf78

// block 0048c0b2
0048c0b2  pop        ecx

// block 0048c0b3
0048c0b3  mov        eax, dword ptr [esi]
0048c0b5  cmp        eax, dword ptr [esi + 8]
0048c0b8  jne        0x48c0c2

// block 0048c0ba
0048c0ba  cmp        dword ptr [esi + 4], edi
0048c0bd  jne        0x48c0d3

// block 0048c0bf
0048c0bf  inc        eax
0048c0c0  mov        dword ptr [esi], eax

// block 0048c0c2
0048c0c2  dec        dword ptr [esi]
0048c0c4  test       byte ptr [esi + 0xc], 0x40
0048c0c8  mov        eax, dword ptr [esi]
0048c0ca  je         0x48c0db

// block 0048c0cc
0048c0cc  cmp        byte ptr [eax], bl
0048c0ce  je         0x48c0dd

// block 0048c0d0
0048c0d0  inc        eax
0048c0d1  mov        dword ptr [esi], eax

// block 0048c0d3
0048c0d3  or         eax, 0xffffffff

// block 0048c0d6
0048c0d6  pop        ebx

// block 0048c0d7
0048c0d7  pop        edi
0048c0d8  pop        esi
0048c0d9  pop        ebp
0048c0da  ret        

// block 0048c0db
0048c0db  mov        byte ptr [eax], bl

// block 0048c0dd
0048c0dd  mov        eax, dword ptr [esi + 0xc]
0048c0e0  inc        dword ptr [esi + 4]
0048c0e3  and        eax, 0xffffffef
0048c0e6  or         eax, 1
0048c0e9  mov        dword ptr [esi + 0xc], eax
0048c0ec  mov        eax, ebx
0048c0ee  and        eax, 0xff
0048c0f3  jmp        0x48c0d6
