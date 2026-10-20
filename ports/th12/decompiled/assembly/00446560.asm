
// block 00446560
00446560  push       ebp
00446561  mov        ebp, esp
00446563  and        esp, 0xfffffff8
00446566  sub        esp, 0x18c
0044656c  mov        eax, dword ptr [0x4ad138]
00446571  xor        eax, esp
00446573  mov        dword ptr [esp + 0x188], eax
0044657a  push       ebx
0044657b  mov        ebx, dword ptr [0x4b4530]
00446581  push       esi
00446582  push       edi
00446583  mov        esi, 1
00446588  lea        edi, [ebx + 0x5a80]
0044658e  mov        edi, edi

// block 00446590
00446590  push       esi
00446591  lea        eax, [esp + 0x154]
00446598  push       0x4a1024
0044659d  push       eax
0044659e  call       0x46cbbb

// block 004465a3
004465a3  add        esp, 0xc
004465a6  lea        ecx, [esp + 0x150]
004465ad  push       ecx
004465ae  call       0x43b6f0

// block 004465b3
004465b3  mov        dword ptr [edi], eax
004465b5  test       byte ptr [ebx + 0x5c18], 4
004465bc  jne        0x4465c7

// block 004465be
004465be  inc        esi
004465bf  add        edi, 4
004465c2  cmp        esi, 0x19
004465c5  jle        0x446590

// block 004465c7
004465c7  push       0x4a129c
004465cc  call       0x46d585

// block 004465d1
004465d1  add        esp, 4
004465d4  push       0x4a129c
004465d9  call       0x46ddb7

// block 004465de
004465de  add        esp, 4
004465e1  lea        edx, [esp + 0x10]
004465e5  push       edx
004465e6  push       0x4a1f74
004465eb  call       dword ptr [0x498078]

// block 004465f1
004465f1  mov        dword ptr [esp + 0xc], eax
004465f5  cmp        eax, -1
004465f8  je         0x446655

// block 004465fa
004465fa  mov        edi, 0x19
004465ff  lea        esi, [ebx + 0x5ae4]

// block 00446605
00446605  push       0x4a1f84
0044660a  call       0x46ddb7

// block 0044660f
0044660f  add        esp, 4
00446612  lea        eax, [esp + 0x3c]
00446616  push       eax
00446617  call       0x43b6f0

// block 0044661c
0044661c  push       0x4a129c
00446621  mov        dword ptr [esi], eax
00446623  call       0x46ddb7

// block 00446628
00446628  add        esp, 4
0044662b  test       byte ptr [ebx + 0x5c18], 4
00446632  jne        0x446651

// block 00446634
00446634  mov        edx, dword ptr [esp + 0xc]
00446638  lea        ecx, [esp + 0x10]
0044663c  push       ecx
0044663d  push       edx
0044663e  call       dword ptr [0x49807c]

// block 00446644
00446644  test       eax, eax
00446646  je         0x446651

// block 00446648
00446648  inc        edi
00446649  add        esi, 4
0044664c  cmp        edi, 0x4b
0044664f  jl         0x446605

// block 00446651
00446651  mov        eax, dword ptr [esp + 0xc]

// block 00446655
00446655  push       eax
00446656  call       dword ptr [0x498080]

// block 0044665c
0044665c  push       0x4a1f84
00446661  call       0x46ddb7

// block 00446666
00446666  mov        eax, dword ptr [ebx + 0x5c18]
0044666c  mov        ecx, dword ptr [esp + 0x198]
00446673  add        esp, 4
00446676  and        eax, 0xfffffffb
00446679  pop        edi
0044667a  or         eax, 8
0044667d  pop        esi
0044667e  mov        dword ptr [ebx + 0x5c18], eax
00446684  pop        ebx
00446685  xor        ecx, esp
00446687  call       0x46c932

// block 0044668c
0044668c  mov        esp, ebp
0044668e  pop        ebp
0044668f  ret        
