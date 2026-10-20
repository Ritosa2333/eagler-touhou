
// block 004517a0
004517a0  sub        esp, 0x154
004517a6  mov        eax, dword ptr [0x4ad138]
004517ab  xor        eax, esp
004517ad  mov        dword ptr [esp + 0x150], eax
004517b4  push       ebx
004517b5  push       ebp
004517b6  mov        ebp, dword ptr [esp + 0x160]
004517bd  push       esi
004517be  test       edi, edi
004517c0  jne        0x4517c9

// block 004517c2
004517c2  xor        eax, eax
004517c4  jmp        0x451898

// block 004517c9
004517c9  xor        ebx, ebx
004517cb  push       ebx
004517cc  call       dword ptr [0x4982f8]

// block 004517d2
004517d2  lea        eax, [esp + 0x10]
004517d6  push       eax
004517d7  push       0x499c6c
004517dc  push       1
004517de  push       ebx
004517df  push       0x49b88c
004517e4  call       dword ptr [0x4982fc]

// block 004517ea
004517ea  test       eax, eax
004517ec  jl         0x451890

// block 004517f2
004517f2  mov        eax, dword ptr [esp + 0x10]
004517f6  mov        ecx, dword ptr [eax]
004517f8  lea        edx, [esp + 0x14]
004517fc  push       edx
004517fd  push       0x49bcfc
00451802  push       eax
00451803  mov        eax, dword ptr [ecx]
00451805  call       eax

// block 00451807
00451807  test       eax, eax
00451809  jl         0x451884

// block 0045180b
0045180b  xor        ecx, ecx
0045180d  mov        eax, 0x104
00451812  mov        edx, 2
00451817  mul        edx
00451819  seto       cl
0045181c  neg        ecx
0045181e  or         ecx, eax
00451820  push       ecx
00451821  call       0x46c9ea

// block 00451826
00451826  add        esp, 4
00451829  push       0x104
0045182e  mov        esi, eax
00451830  push       esi
00451831  push       -1
00451833  push       ebp
00451834  push       ebx
00451835  push       ebx
00451836  call       dword ptr [0x4980dc]

// block 0045183c
0045183c  mov        eax, dword ptr [esp + 0x14]
00451840  mov        ecx, dword ptr [eax]
00451842  mov        edx, dword ptr [ecx + 0x14]
00451845  push       ebx
00451846  push       esi
00451847  push       eax
00451848  call       edx

// block 0045184a
0045184a  test       eax, eax
0045184c  jl         0x45186f

// block 0045184e
0045184e  mov        eax, dword ptr [esp + 0x10]
00451852  mov        ecx, dword ptr [eax]
00451854  push       ebx
00451855  lea        edx, [esp + 0x1c]
00451859  push       edx
0045185a  push       0x104
0045185f  push       edi
00451860  push       eax
00451861  mov        eax, dword ptr [ecx + 0xc]
00451864  call       eax

// block 00451866
00451866  test       eax, eax
00451868  jl         0x45186f

// block 0045186a
0045186a  mov        ebx, 1

// block 0045186f
0045186f  push       esi
00451870  call       0x46ca4f

// block 00451875
00451875  mov        eax, dword ptr [esp + 0x18]
00451879  mov        ecx, dword ptr [eax]
0045187b  mov        edx, dword ptr [ecx + 8]
0045187e  add        esp, 4
00451881  push       eax
00451882  call       edx

// block 00451884
00451884  mov        eax, dword ptr [esp + 0x10]
00451888  mov        ecx, dword ptr [eax]
0045188a  mov        edx, dword ptr [ecx + 8]
0045188d  push       eax
0045188e  call       edx

// block 00451890
00451890  call       dword ptr [0x4982f4]

// block 00451896
00451896  mov        eax, ebx

// block 00451898
00451898  mov        ecx, dword ptr [esp + 0x15c]
0045189f  pop        esi
004518a0  pop        ebp
004518a1  pop        ebx
004518a2  xor        ecx, esp
004518a4  call       0x46c932

// block 004518a9
004518a9  add        esp, 0x154
004518af  ret        4
