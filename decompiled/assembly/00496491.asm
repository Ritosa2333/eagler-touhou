
// block 00496491
00496491  mov        edi, edi
00496493  push       ebp
00496494  mov        ebp, esp
00496496  sub        esp, 0x14
00496499  mov        eax, dword ptr [ebp + 8]
0049649c  push       ebx
0049649d  push       esi
0049649e  xor        ebx, ebx
004964a0  mov        esi, eax
004964a2  and        esi, 0x1f
004964a5  inc        ebx
004964a6  mov        dword ptr [ebp - 4], esi
004964a9  test       al, 8
004964ab  je         0x4964c1

// block 004964ad
004964ad  test       byte ptr [ebp + 0x10], bl
004964b0  je         0x4964c1

// block 004964b2
004964b2  push       ebx
004964b3  call       0x4911aa

// block 004964b8
004964b8  pop        ecx
004964b9  and        esi, 0xfffffff7
004964bc  jmp        0x496651

// block 004964c1
004964c1  test       al, 4
004964c3  je         0x4964db

// block 004964c5
004964c5  test       byte ptr [ebp + 0x10], 4
004964c9  je         0x4964db

// block 004964cb
004964cb  push       4
004964cd  call       0x4911aa

// block 004964d2
004964d2  pop        ecx
004964d3  and        esi, 0xfffffffb
004964d6  jmp        0x496651

// block 004964db
004964db  test       bl, al
004964dd  je         0x49657d

// block 004964e3
004964e3  test       byte ptr [ebp + 0x10], 8
004964e7  je         0x49657d

// block 004964ed
004964ed  push       8
004964ef  call       0x4911aa

// block 004964f4
004964f4  mov        eax, dword ptr [ebp + 0x10]
004964f7  pop        ecx
004964f8  mov        ecx, 0xc00
004964fd  and        eax, ecx
004964ff  je         0x496555

// block 00496501
00496501  cmp        eax, 0x400
00496506  je         0x49653f

// block 00496508
00496508  cmp        eax, 0x800
0049650d  je         0x496529

// block 0049650f
0049650f  cmp        eax, ecx
00496511  jne        0x496575

// block 00496513
00496513  fldz       
00496515  mov        ecx, dword ptr [ebp + 0xc]
00496518  fcomp      qword ptr [ecx]
0049651a  fnstsw     ax
0049651c  fld        qword ptr [0x4b38a0]
00496522  test       ah, 5
00496525  jnp        0x496573

// block 00496527
00496527  jmp        0x496571

// block 00496529
00496529  fldz       
0049652b  mov        ecx, dword ptr [ebp + 0xc]
0049652e  fcomp      qword ptr [ecx]
00496530  fnstsw     ax
00496532  test       ah, 5
00496535  jnp        0x496563

// block 00496537
00496537  fld        qword ptr [0x4b38a0]
0049653d  jmp        0x496571

// block 0049653f
0049653f  fldz       
00496541  mov        ecx, dword ptr [ebp + 0xc]
00496544  fcomp      qword ptr [ecx]
00496546  fnstsw     ax
00496548  test       ah, 5
0049654b  jp         0x49656b

// block 0049654d
0049654d  fld        qword ptr [0x4b38a0]
00496553  jmp        0x496573

// block 00496555
00496555  fldz       
00496557  mov        ecx, dword ptr [ebp + 0xc]
0049655a  fcomp      qword ptr [ecx]
0049655c  fnstsw     ax
0049655e  test       ah, 5
00496561  jp         0x49656b

// block 00496563
00496563  fld        qword ptr [0x4b3890]
00496569  jmp        0x496573

// block 0049656b
0049656b  fld        qword ptr [0x4b3890]

// block 00496571
00496571  fchs       

// block 00496573
00496573  fstp       qword ptr [ecx]

// block 00496575
00496575  and        esi, 0xfffffffe
00496578  jmp        0x496651

// block 0049657d
0049657d  test       al, 2
0049657f  je         0x496651

// block 00496585
00496585  test       byte ptr [ebp + 0x10], 0x10
00496589  je         0x496651

// block 0049658f
0049658f  xor        esi, esi
00496591  test       al, 0x10
00496593  je         0x496597

// block 00496595
00496595  mov        esi, ebx

// block 00496597
00496597  fldz       
00496599  push       edi
0049659a  mov        edi, dword ptr [ebp + 0xc]
0049659d  fcomp      qword ptr [edi]
0049659f  fnstsw     ax
004965a1  test       ah, 0x44
004965a4  jnp        0x49663b

// block 004965aa
004965aa  fld        qword ptr [edi]
004965ac  lea        eax, [ebp - 8]
004965af  push       eax
004965b0  push       ecx
004965b1  push       ecx
004965b2  fstp       qword ptr [esp]
004965b5  call       0x496afd

// block 004965ba
004965ba  mov        ecx, dword ptr [ebp - 8]
004965bd  fstp       qword ptr [ebp - 0x14]
004965c0  add        ecx, 0xfffffa00
004965c6  add        esp, 0xc
004965c9  cmp        ecx, 0xfffffbce
004965cf  jge        0x4965de

// block 004965d1
004965d1  fld        qword ptr [ebp - 0x14]
004965d4  mov        esi, ebx
004965d6  fmul       qword ptr [0x4a3d58]
004965dc  jmp        0x496631

// block 004965de
004965de  fldz       
004965e0  fcomp      qword ptr [ebp - 0x14]
004965e3  fnstsw     ax
004965e5  test       ah, 0x41
004965e8  jne        0x4965ee

// block 004965ea
004965ea  mov        edx, ebx
004965ec  jmp        0x4965f0

// block 004965ee
004965ee  xor        edx, edx

// block 004965f0
004965f0  mov        eax, dword ptr [ebp - 0xe]
004965f3  and        eax, 0xf
004965f6  or         eax, 0x10
004965f9  mov        word ptr [ebp - 0xe], ax
004965fd  mov        eax, 0xfffffc03
00496602  cmp        ecx, eax
00496604  jge        0x496628

// block 00496606
00496606  sub        eax, ecx

// block 00496608
00496608  test       byte ptr [ebp - 0x14], bl
0049660b  je         0x496613

// block 0049660d
0049660d  test       esi, esi
0049660f  jne        0x496613

// block 00496611
00496611  mov        esi, ebx

// block 00496613
00496613  shr        dword ptr [ebp - 0x14], 1
00496616  test       byte ptr [ebp - 0x10], bl
00496619  je         0x496622

// block 0049661b
0049661b  or         dword ptr [ebp - 0x14], 0x80000000

// block 00496622
00496622  shr        dword ptr [ebp - 0x10], 1
00496625  dec        eax
00496626  jne        0x496608

// block 00496628
00496628  test       edx, edx
0049662a  je         0x496634

// block 0049662c
0049662c  fld        qword ptr [ebp - 0x14]
0049662f  fchs       

// block 00496631
00496631  fstp       qword ptr [ebp - 0x14]

// block 00496634
00496634  fld        qword ptr [ebp - 0x14]
00496637  fstp       qword ptr [edi]
00496639  jmp        0x49663d

// block 0049663b
0049663b  mov        esi, ebx

// block 0049663d
0049663d  pop        edi
0049663e  test       esi, esi
00496640  je         0x49664a

// block 00496642
00496642  push       0x10
00496644  call       0x4911aa

// block 00496649
00496649  pop        ecx

// block 0049664a
0049664a  and        dword ptr [ebp - 4], 0xfffffffd
0049664e  mov        esi, dword ptr [ebp - 4]

// block 00496651
00496651  test       byte ptr [ebp + 8], 0x10
00496655  je         0x496668

// block 00496657
00496657  test       byte ptr [ebp + 0x10], 0x20
0049665b  je         0x496668

// block 0049665d
0049665d  push       0x20
0049665f  call       0x4911aa

// block 00496664
00496664  pop        ecx
00496665  and        esi, 0xffffffef

// block 00496668
00496668  xor        eax, eax
0049666a  test       esi, esi
0049666c  pop        esi
0049666d  sete       al
00496670  pop        ebx
00496671  leave      
00496672  ret        
