
// block 0048e9f9
0048e9f9  push       0x10
0048e9fb  push       0x4ab188
0048ea00  call       0x46fcec

// block 0048ea05
0048ea05  mov        eax, dword ptr [ebp + 8]
0048ea08  cmp        eax, -2
0048ea0b  jne        0x48ea28

// block 0048ea0d
0048ea0d  call       0x46e982

// block 0048ea12
0048ea12  and        dword ptr [eax], 0
0048ea15  call       0x46e96f

// block 0048ea1a
0048ea1a  mov        dword ptr [eax], 9

// block 0048ea20
0048ea20  or         eax, 0xffffffff
0048ea23  jmp        0x48eae6

// block 0048ea28
0048ea28  xor        esi, esi
0048ea2a  cmp        eax, esi
0048ea2c  jl         0x48ea36

// block 0048ea2e
0048ea2e  cmp        eax, dword ptr [0x4d6308]
0048ea34  jb         0x48ea57

// block 0048ea36
0048ea36  call       0x46e982

// block 0048ea3b
0048ea3b  mov        dword ptr [eax], esi
0048ea3d  call       0x46e96f

// block 0048ea42
0048ea42  mov        dword ptr [eax], 9

// block 0048ea48
0048ea48  push       esi
0048ea49  push       esi
0048ea4a  push       esi
0048ea4b  push       esi
0048ea4c  push       esi
0048ea4d  call       0x470f2d

// block 0048ea52
0048ea52  add        esp, 0x14
0048ea55  jmp        0x48ea20

// block 0048ea57
0048ea57  mov        ecx, eax
0048ea59  sar        ecx, 5
0048ea5c  lea        ebx, [ecx*4 + 0x4d6320]
0048ea63  mov        edi, eax
0048ea65  and        edi, 0x1f
0048ea68  shl        edi, 6
0048ea6b  mov        ecx, dword ptr [ebx]
0048ea6d  movsx      ecx, byte ptr [ecx + edi + 4]
0048ea72  and        ecx, 1
0048ea75  je         0x48ea36

// block 0048ea77
0048ea77  mov        ecx, 0x7fffffff
0048ea7c  cmp        ecx, dword ptr [ebp + 0x10]
0048ea7f  sbb        ecx, ecx
0048ea81  inc        ecx
0048ea82  jne        0x48ea98

// block 0048ea84
0048ea84  call       0x46e982

// block 0048ea89
0048ea89  mov        dword ptr [eax], esi
0048ea8b  call       0x46e96f

// block 0048ea90
0048ea90  mov        dword ptr [eax], 0x16
0048ea96  jmp        0x48ea48

// block 0048ea98
0048ea98  push       eax
0048ea99  call       0x48cb3d

// block 0048ea9e
0048ea9e  pop        ecx
0048ea9f  mov        dword ptr [ebp - 4], esi
0048eaa2  mov        eax, dword ptr [ebx]
0048eaa4  test       byte ptr [eax + edi + 4], 1
0048eaa9  je         0x48eac1

// block 0048eaab
0048eaab  push       dword ptr [ebp + 0x10]
0048eaae  push       dword ptr [ebp + 0xc]
0048eab1  push       dword ptr [ebp + 8]
0048eab4  call       0x48e437

// block 0048eab9
0048eab9  add        esp, 0xc
0048eabc  mov        dword ptr [ebp - 0x1c], eax
0048eabf  jmp        0x48ead7

// block 0048eac1
0048eac1  call       0x46e96f

// block 0048eac6
0048eac6  mov        dword ptr [eax], 9
0048eacc  call       0x46e982

// block 0048ead1
0048ead1  mov        dword ptr [eax], esi
0048ead3  or         dword ptr [ebp - 0x1c], 0xffffffff

// block 0048ead7
0048ead7  mov        dword ptr [ebp - 4], 0xfffffffe
0048eade  call       0x48eaec

// block 0048eae3
0048eae3  mov        eax, dword ptr [ebp - 0x1c]

// block 0048eae6
0048eae6  call       0x46fd31

// block 0048eaeb
0048eaeb  ret        
