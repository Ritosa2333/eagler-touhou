
// block 00491062
00491062  push       0x10
00491064  push       0x4ab1e8
00491069  call       0x46fcec

// block 0049106e
0049106e  mov        eax, dword ptr [ebp + 8]
00491071  cmp        eax, -2
00491074  jne        0x491091

// block 00491076
00491076  call       0x46e982

// block 0049107b
0049107b  and        dword ptr [eax], 0
0049107e  call       0x46e96f

// block 00491083
00491083  mov        dword ptr [eax], 9

// block 00491089
00491089  or         eax, 0xffffffff
0049108c  jmp        0x49111f

// block 00491091
00491091  xor        edi, edi
00491093  cmp        eax, edi
00491095  jl         0x49109f

// block 00491097
00491097  cmp        eax, dword ptr [0x4d6308]
0049109d  jb         0x4910c0

// block 0049109f
0049109f  call       0x46e982

// block 004910a4
004910a4  mov        dword ptr [eax], edi
004910a6  call       0x46e96f

// block 004910ab
004910ab  mov        dword ptr [eax], 9
004910b1  push       edi
004910b2  push       edi
004910b3  push       edi
004910b4  push       edi
004910b5  push       edi
004910b6  call       0x470f2d

// block 004910bb
004910bb  add        esp, 0x14
004910be  jmp        0x491089

// block 004910c0
004910c0  mov        ecx, eax
004910c2  sar        ecx, 5
004910c5  lea        ebx, [ecx*4 + 0x4d6320]
004910cc  mov        esi, eax
004910ce  and        esi, 0x1f
004910d1  shl        esi, 6
004910d4  mov        ecx, dword ptr [ebx]
004910d6  movsx      ecx, byte ptr [ecx + esi + 4]
004910db  and        ecx, 1
004910de  je         0x49109f

// block 004910e0
004910e0  push       eax
004910e1  call       0x48cb3d

// block 004910e6
004910e6  pop        ecx
004910e7  mov        dword ptr [ebp - 4], edi
004910ea  mov        eax, dword ptr [ebx]
004910ec  test       byte ptr [eax + esi + 4], 1
004910f1  je         0x491101

// block 004910f3
004910f3  push       dword ptr [ebp + 8]
004910f6  call       0x490fc6

// block 004910fb
004910fb  pop        ecx
004910fc  mov        dword ptr [ebp - 0x1c], eax
004910ff  jmp        0x491110

// block 00491101
00491101  call       0x46e96f

// block 00491106
00491106  mov        dword ptr [eax], 9
0049110c  or         dword ptr [ebp - 0x1c], 0xffffffff

// block 00491110
00491110  mov        dword ptr [ebp - 4], 0xfffffffe
00491117  call       0x491125

// block 0049111c
0049111c  mov        eax, dword ptr [ebp - 0x1c]

// block 0049111f
0049111f  call       0x46fd31

// block 00491124
00491124  ret        
