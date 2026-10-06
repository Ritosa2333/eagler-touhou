
// block 0047b650
0047b650  push       0x14
0047b652  push       0x4aae78
0047b657  call       0x46fcec

// block 0047b65c
0047b65c  or         esi, 0xffffffff
0047b65f  mov        dword ptr [ebp - 0x24], esi
0047b662  mov        dword ptr [ebp - 0x20], esi
0047b665  mov        eax, dword ptr [ebp + 8]
0047b668  cmp        eax, -2
0047b66b  jne        0x47b689

// block 0047b66d
0047b66d  call       0x46e982

// block 0047b672
0047b672  and        dword ptr [eax], 0
0047b675  call       0x46e96f

// block 0047b67a
0047b67a  mov        dword ptr [eax], 9

// block 0047b680
0047b680  mov        eax, esi
0047b682  mov        edx, esi
0047b684  jmp        0x47b759

// block 0047b689
0047b689  xor        edi, edi
0047b68b  cmp        eax, edi
0047b68d  jl         0x47b697

// block 0047b68f
0047b68f  cmp        eax, dword ptr [0x4d6308]
0047b695  jb         0x47b6b8

// block 0047b697
0047b697  call       0x46e982

// block 0047b69c
0047b69c  mov        dword ptr [eax], edi
0047b69e  call       0x46e96f

// block 0047b6a3
0047b6a3  mov        dword ptr [eax], 9
0047b6a9  push       edi
0047b6aa  push       edi
0047b6ab  push       edi
0047b6ac  push       edi
0047b6ad  push       edi
0047b6ae  call       0x470f2d

// block 0047b6b3
0047b6b3  add        esp, 0x14
0047b6b6  jmp        0x47b680

// block 0047b6b8
0047b6b8  mov        ecx, eax
0047b6ba  sar        ecx, 5
0047b6bd  lea        ebx, [ecx*4 + 0x4d6320]
0047b6c4  mov        esi, eax
0047b6c6  and        esi, 0x1f
0047b6c9  shl        esi, 6
0047b6cc  mov        ecx, dword ptr [ebx]
0047b6ce  movsx      ecx, byte ptr [ecx + esi + 4]
0047b6d3  and        ecx, 1
0047b6d6  jne        0x47b6fe

// block 0047b6d8
0047b6d8  call       0x46e982

// block 0047b6dd
0047b6dd  mov        dword ptr [eax], edi
0047b6df  call       0x46e96f

// block 0047b6e4
0047b6e4  mov        dword ptr [eax], 9
0047b6ea  push       edi
0047b6eb  push       edi
0047b6ec  push       edi
0047b6ed  push       edi
0047b6ee  push       edi
0047b6ef  call       0x470f2d

// block 0047b6f4
0047b6f4  add        esp, 0x14
0047b6f7  or         edx, 0xffffffff
0047b6fa  mov        eax, edx
0047b6fc  jmp        0x47b759

// block 0047b6fe
0047b6fe  push       eax
0047b6ff  call       0x48cb3d

// block 0047b704
0047b704  pop        ecx
0047b705  mov        dword ptr [ebp - 4], edi
0047b708  mov        eax, dword ptr [ebx]
0047b70a  test       byte ptr [eax + esi + 4], 1
0047b70f  je         0x47b72d

// block 0047b711
0047b711  push       dword ptr [ebp + 0x14]
0047b714  push       dword ptr [ebp + 0x10]
0047b717  push       dword ptr [ebp + 0xc]
0047b71a  push       dword ptr [ebp + 8]
0047b71d  call       0x47b5cb

// block 0047b722
0047b722  add        esp, 0x10
0047b725  mov        dword ptr [ebp - 0x24], eax
0047b728  mov        dword ptr [ebp - 0x20], edx
0047b72b  jmp        0x47b747

// block 0047b72d
0047b72d  call       0x46e96f

// block 0047b732
0047b732  mov        dword ptr [eax], 9
0047b738  call       0x46e982

// block 0047b73d
0047b73d  mov        dword ptr [eax], edi
0047b73f  or         dword ptr [ebp - 0x24], 0xffffffff
0047b743  or         dword ptr [ebp - 0x20], 0xffffffff

// block 0047b747
0047b747  mov        dword ptr [ebp - 4], 0xfffffffe
0047b74e  call       0x47b75f

// block 0047b753
0047b753  mov        eax, dword ptr [ebp - 0x24]
0047b756  mov        edx, dword ptr [ebp - 0x20]

// block 0047b759
0047b759  call       0x46fd31

// block 0047b75e
0047b75e  ret        
