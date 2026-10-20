
// block 004105e0
004105e0  sub        esp, 8
004105e3  push       ebx
004105e4  push       ebp
004105e5  push       esi
004105e6  push       edi
004105e7  mov        esi, ecx
004105e9  push       0xd8
004105ee  mov        ebp, edx
004105f0  mov        dword ptr [esp + 0x18], esi
004105f4  call       0x46d04a

// block 004105f9
004105f9  push       0xd8
004105fe  mov        edi, eax
00410600  xor        ebx, ebx
00410602  push       ebx
00410603  push       edi
00410604  mov        dword ptr [esi + 0x478], edi
0041060a  call       0x477420

// block 0041060f
0041060f  fld        dword ptr [ebp]
00410612  fstp       dword ptr [edi]
00410614  add        esp, 0x10
00410617  fld        dword ptr [ebp + 4]
0041061a  mov        esi, 0x4ce568
0041061f  fstp       dword ptr [edi + 4]
00410622  call       0x464440

// block 00410627
00410627  mov        dword ptr [esp + 0x10], eax
0041062b  fild       dword ptr [esp + 0x10]
0041062f  test       eax, eax
00410631  jge        0x410639

// block 00410633
00410633  fadd       dword ptr [0x4a3e10]

// block 00410639
00410639  fmul       qword ptr [0x4a3ea8]
0041063f  fsub       qword ptr [0x4a3ce0]
00410645  fstp       dword ptr [esp + 0x10]
00410649  fld        dword ptr [esp + 0x10]
0041064d  fmul       qword ptr [0x4a3cd8]
00410653  fstp       dword ptr [edi + 0xc0]
00410659  mov        eax, dword ptr [edi + 0xd4]
0041065f  test       al, 1
00410661  jne        0x41068e

// block 00410663
00410663  fldz       
00410665  or         eax, 1
00410668  fstp       dword ptr [edi + 0xcc]
0041066e  mov        dword ptr [edi + 0xc8], ebx
00410674  mov        dword ptr [edi + 0xc4], 0xfff0bdc1
0041067e  mov        dword ptr [edi + 0xd0], 0x4b2ed0
00410688  mov        dword ptr [edi + 0xd4], eax

// block 0041068e
0041068e  fld1       
00410690  mov        dword ptr [edi + 0xc8], 1
0041069a  fstp       dword ptr [edi + 0xcc]
004106a0  mov        dword ptr [edi + 0xc4], ebx
004106a6  mov        dword ptr [esp + 0x10], ebx
004106aa  xor        ecx, ecx
004106ac  lea        eax, [edi + 0x80]

// block 004106b2
004106b2  or         bl, 0xff
004106b5  cmp        ecx, 8
004106b8  mov        dword ptr [eax], 0xff40ff00
004106be  jge        0x4106cb

// block 004106c0
004106c0  mov        dl, cl
004106c2  shl        dl, 5
004106c5  sub        bl, dl
004106c7  mov        byte ptr [eax], bl
004106c9  jmp        0x4106db

// block 004106cb
004106cb  mov        dl, byte ptr [esp + 0x10]
004106cf  shl        dl, 4
004106d2  sub        bl, dl
004106d4  inc        dword ptr [esp + 0x10]
004106d8  mov        byte ptr [eax + 3], bl

// block 004106db
004106db  inc        ecx
004106dc  add        eax, 4
004106df  cmp        ecx, 0x10
004106e2  jl         0x4106b2

// block 004106e4
004106e4  mov        ecx, dword ptr [ebp]
004106e7  mov        eax, dword ptr [esp + 0x14]
004106eb  mov        dword ptr [eax + 0x430], ecx
004106f1  mov        edx, dword ptr [ebp + 4]
004106f4  pop        edi
004106f5  mov        dword ptr [eax + 0x434], edx
004106fb  mov        ecx, dword ptr [ebp + 8]
004106fe  pop        esi
004106ff  pop        ebp
00410700  mov        dword ptr [eax + 0x438], ecx
00410706  mov        dword ptr [eax + 0x20], 0xd
0041070d  xor        eax, eax
0041070f  pop        ebx
00410710  add        esp, 8
00410713  ret        
