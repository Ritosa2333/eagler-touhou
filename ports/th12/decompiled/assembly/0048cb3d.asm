
// block 0048cb3d
0048cb3d  push       0xc
0048cb3f  push       0x4ab058
0048cb44  call       0x46fcec

// block 0048cb49
0048cb49  mov        edi, dword ptr [ebp + 8]
0048cb4c  mov        eax, edi
0048cb4e  sar        eax, 5
0048cb51  mov        esi, edi
0048cb53  and        esi, 0x1f
0048cb56  shl        esi, 6
0048cb59  add        esi, dword ptr [eax*4 + 0x4d6320]
0048cb60  mov        dword ptr [ebp - 0x1c], 1
0048cb67  xor        ebx, ebx
0048cb69  cmp        dword ptr [esi + 8], ebx
0048cb6c  jne        0x48cba4

// block 0048cb6e
0048cb6e  push       0xa
0048cb70  call       0x46ec9a

// block 0048cb75
0048cb75  pop        ecx
0048cb76  mov        dword ptr [ebp - 4], ebx
0048cb79  cmp        dword ptr [esi + 8], ebx
0048cb7c  jne        0x48cb98

// block 0048cb7e
0048cb7e  push       0xfa0
0048cb83  lea        eax, [esi + 0xc]
0048cb86  push       eax
0048cb87  call       0x47b416

// block 0048cb8c
0048cb8c  pop        ecx
0048cb8d  pop        ecx
0048cb8e  test       eax, eax
0048cb90  jne        0x48cb95

// block 0048cb92
0048cb92  mov        dword ptr [ebp - 0x1c], ebx

// block 0048cb95
0048cb95  inc        dword ptr [esi + 8]

// block 0048cb98
0048cb98  mov        dword ptr [ebp - 4], 0xfffffffe
0048cb9f  call       0x48cbd4

// block 0048cba4
0048cba4  cmp        dword ptr [ebp - 0x1c], ebx
0048cba7  je         0x48cbc6

// block 0048cba9
0048cba9  mov        eax, edi
0048cbab  sar        eax, 5
0048cbae  and        edi, 0x1f
0048cbb1  shl        edi, 6
0048cbb4  mov        eax, dword ptr [eax*4 + 0x4d6320]
0048cbbb  lea        eax, [eax + edi + 0xc]
0048cbbf  push       eax
0048cbc0  call       dword ptr [0x498088]

// block 0048cbc6
0048cbc6  mov        eax, dword ptr [ebp - 0x1c]
0048cbc9  call       0x46fd31

// block 0048cbce
0048cbce  ret        
