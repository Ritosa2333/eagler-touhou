
// block 00451530
00451530  sub        esp, 0x264
00451536  mov        eax, dword ptr [0x4ad138]
0045153b  xor        eax, esp
0045153d  mov        dword ptr [esp + 0x260], eax
00451544  push       esi
00451545  push       edi
00451546  push       0x40
00451548  lea        eax, [esp + 0x14]
0045154c  push       0
0045154e  push       eax
0045154f  mov        dword ptr [esp + 0x18], 0x44
00451557  call       0x477420

// block 0045155c
0045155c  add        esp, 0xc
0045155f  push       0x105
00451564  lea        ecx, [esp + 0x160]
0045156b  push       ecx
0045156c  push       0
0045156e  call       dword ptr [0x4980b4]

// block 00451574
00451574  push       0x105
00451579  lea        edx, [esp + 0x58]
0045157d  push       edx
0045157e  call       dword ptr [0x4980d8]

// block 00451584
00451584  lea        eax, [esp + 0xc]
00451588  push       eax
00451589  call       dword ptr [0x4981bc]

// block 0045158f
0045158f  mov        eax, dword ptr [esp + 0x18]
00451593  test       eax, eax
00451595  je         0x45165e

// block 0045159b
0045159b  push       0x2e
0045159d  push       eax
0045159e  call       0x46d260

// block 004515a3
004515a3  mov        ecx, dword ptr [esp + 0x20]
004515a7  add        esp, 8
004515aa  push       ecx
004515ab  mov        esi, eax
004515ad  call       0x463df0

// block 004515b2
004515b2  test       eax, eax
004515b4  je         0x451655

// block 004515ba
004515ba  test       esi, esi
004515bc  je         0x451655

// block 004515c2
004515c2  push       0x4a299c
004515c7  push       esi
004515c8  call       0x46e3f8

// block 004515cd
004515cd  add        esp, 8
004515d0  test       eax, eax
004515d2  jne        0x451600

// block 004515d4
004515d4  mov        edx, dword ptr [esp + 0x18]
004515d8  push       edx
004515d9  lea        edi, [esp + 0x58]
004515dd  call       0x4517a0

// block 004515e2
004515e2  mov        eax, edi
004515e4  push       0x2e
004515e6  push       eax
004515e7  call       0x46d260

// block 004515ec
004515ec  push       0x4a299c
004515f1  push       eax
004515f2  call       0x46e3f8

// block 004515f7
004515f7  add        esp, 0x10
004515fa  test       eax, eax
004515fc  je         0x4515d4

// block 004515fe
004515fe  jmp        0x45161a

// block 00451600
00451600  mov        eax, dword ptr [esp + 0x18]
00451604  lea        edx, [esp + 0x54]
00451608  sub        edx, eax
0045160a  lea        ebx, [ebx]

// block 00451610
00451610  mov        cl, byte ptr [eax]
00451612  mov        byte ptr [edx + eax], cl
00451615  inc        eax
00451616  test       cl, cl
00451618  jne        0x451610

// block 0045161a
0045161a  lea        edx, [esp + 0x54]
0045161e  lea        ecx, [esp + 0x15c]

// block 00451625
00451625  mov        al, byte ptr [ecx]
00451627  cmp        al, byte ptr [edx]
00451629  jne        0x451645

// block 0045162b
0045162b  test       al, al
0045162d  je         0x451641

// block 0045162f
0045162f  mov        al, byte ptr [ecx + 1]
00451632  cmp        al, byte ptr [edx + 1]
00451635  jne        0x451645

// block 00451637
00451637  add        ecx, 2
0045163a  add        edx, 2
0045163d  test       al, al
0045163f  jne        0x451625

// block 00451641
00451641  xor        eax, eax
00451643  jmp        0x45164a

// block 00451645
00451645  sbb        eax, eax
00451647  sbb        eax, -1

// block 0045164a
0045164a  test       eax, eax
0045164c  je         0x451655

// block 0045164e
0045164e  mov        byte ptr [0x4cf418], 1

// block 00451655
00451655  and        dword ptr [0x4cee78], 0xffffffbf
0045165c  jmp        0x451665

// block 0045165e
0045165e  or         dword ptr [0x4cee78], 0x40

// block 00451665
00451665  mov        eax, dword ptr [0x4d4ea8]
0045166a  mov        ecx, dword ptr [esp + 0x268]
00451671  neg        eax
00451673  sbb        eax, eax
00451675  pop        edi
00451676  neg        eax
00451678  pop        esi
00451679  xor        ecx, esp
0045167b  dec        eax
0045167c  call       0x46c932

// block 00451681
00451681  add        esp, 0x264
00451687  ret        
