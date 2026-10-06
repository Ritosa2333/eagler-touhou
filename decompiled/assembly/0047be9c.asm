
// block 0047be9c
0047be9c  push       0x10
0047be9e  push       0x4aae98
0047bea3  call       0x46fcec

// block 0047bea8
0047bea8  mov        eax, dword ptr [ebp + 8]
0047beab  cmp        eax, -2
0047beae  jne        0x47becb

// block 0047beb0
0047beb0  call       0x46e982

// block 0047beb5
0047beb5  and        dword ptr [eax], 0
0047beb8  call       0x46e96f

// block 0047bebd
0047bebd  mov        dword ptr [eax], 9

// block 0047bec3
0047bec3  or         eax, 0xffffffff
0047bec6  jmp        0x47bf68

// block 0047becb
0047becb  xor        edi, edi
0047becd  cmp        eax, edi
0047becf  jl         0x47bed9

// block 0047bed1
0047bed1  cmp        eax, dword ptr [0x4d6308]
0047bed7  jb         0x47befa

// block 0047bed9
0047bed9  call       0x46e982

// block 0047bede
0047bede  mov        dword ptr [eax], edi
0047bee0  call       0x46e96f

// block 0047bee5
0047bee5  mov        dword ptr [eax], 9
0047beeb  push       edi
0047beec  push       edi
0047beed  push       edi
0047beee  push       edi
0047beef  push       edi
0047bef0  call       0x470f2d

// block 0047bef5
0047bef5  add        esp, 0x14
0047bef8  jmp        0x47bec3

// block 0047befa
0047befa  mov        ecx, eax
0047befc  sar        ecx, 5
0047beff  lea        ebx, [ecx*4 + 0x4d6320]
0047bf06  mov        esi, eax
0047bf08  and        esi, 0x1f
0047bf0b  shl        esi, 6
0047bf0e  mov        ecx, dword ptr [ebx]
0047bf10  movsx      ecx, byte ptr [ecx + esi + 4]
0047bf15  and        ecx, 1
0047bf18  je         0x47bed9

// block 0047bf1a
0047bf1a  push       eax
0047bf1b  call       0x48cb3d

// block 0047bf20
0047bf20  pop        ecx
0047bf21  mov        dword ptr [ebp - 4], edi
0047bf24  mov        eax, dword ptr [ebx]
0047bf26  test       byte ptr [eax + esi + 4], 1
0047bf2b  je         0x47bf43

// block 0047bf2d
0047bf2d  push       dword ptr [ebp + 0x10]
0047bf30  push       dword ptr [ebp + 0xc]
0047bf33  push       dword ptr [ebp + 8]
0047bf36  call       0x47b769

// block 0047bf3b
0047bf3b  add        esp, 0xc
0047bf3e  mov        dword ptr [ebp - 0x1c], eax
0047bf41  jmp        0x47bf59

// block 0047bf43
0047bf43  call       0x46e96f

// block 0047bf48
0047bf48  mov        dword ptr [eax], 9
0047bf4e  call       0x46e982

// block 0047bf53
0047bf53  mov        dword ptr [eax], edi
0047bf55  or         dword ptr [ebp - 0x1c], 0xffffffff

// block 0047bf59
0047bf59  mov        dword ptr [ebp - 4], 0xfffffffe
0047bf60  call       0x47bf6e

// block 0047bf65
0047bf65  mov        eax, dword ptr [ebp - 0x1c]

// block 0047bf68
0047bf68  call       0x46fd31

// block 0047bf6d
0047bf6d  ret        
