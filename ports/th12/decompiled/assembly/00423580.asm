
// block 00423580
00423580  mov        eax, dword ptr [esp + 4]
00423584  push       ebx
00423585  mov        ebx, dword ptr [0x498088]
0042358b  push       ebp
0042358c  mov        ebp, dword ptr [0x49808c]
00423592  push       esi
00423593  mov        esi, dword ptr [eax + 8]
00423596  push       edi
00423597  mov        edi, dword ptr [0x4ce89c]
0042359d  test       esi, esi
0042359f  je         0x4235dc

// block 004235a1
004235a1  test       dword ptr [0x4cee78], 0x8000
004235ab  je         0x4235ba

// block 004235ad
004235ad  push       0x4cf0f8
004235b2  call       ebx

// block 004235b4
004235b4  inc        byte ptr [0x4cf218]

// block 004235ba
004235ba  mov        ecx, esi
004235bc  mov        edx, edi
004235be  call       0x462890

// block 004235c3
004235c3  test       dword ptr [0x4cee78], 0x8000
004235cd  je         0x4235dc

// block 004235cf
004235cf  push       0x4cf0f8
004235d4  call       ebp

// block 004235d6
004235d6  dec        byte ptr [0x4cf218]

// block 004235dc
004235dc  mov        ecx, dword ptr [esp + 0x14]
004235e0  mov        esi, dword ptr [ecx + 0xc]
004235e3  mov        edi, dword ptr [0x4ce89c]
004235e9  test       esi, esi
004235eb  je         0x423628

// block 004235ed
004235ed  test       dword ptr [0x4cee78], 0x8000
004235f7  je         0x423606

// block 004235f9
004235f9  push       0x4cf0f8
004235fe  call       ebx

// block 00423600
00423600  inc        byte ptr [0x4cf218]

// block 00423606
00423606  mov        ecx, esi
00423608  mov        edx, edi
0042360a  call       0x462890

// block 0042360f
0042360f  test       dword ptr [0x4cee78], 0x8000
00423619  je         0x423628

// block 0042361b
0042361b  push       0x4cf0f8
00423620  call       ebp

// block 00423622
00423622  dec        byte ptr [0x4cf218]

// block 00423628
00423628  mov        edx, dword ptr [esp + 0x14]
0042362c  mov        edi, dword ptr [0x4ce8cc]
00423632  add        edx, 0xdc
00423638  mov        ebx, 0xa
0042363d  mov        esi, 0x10000000

// block 00423642
00423642  mov        ecx, dword ptr [edx]
00423644  test       ecx, ecx
00423646  je         0x4236af

// block 00423648
00423648  mov        eax, dword ptr [edi + 0x8856b8]
0042364e  test       eax, eax
00423650  je         0x423660

// block 00423652
00423652  mov        ebp, dword ptr [eax]
00423654  cmp        dword ptr [ebp], ecx
00423657  je         0x423680

// block 00423659
00423659  mov        eax, dword ptr [eax + 4]
0042365c  test       eax, eax
0042365e  jne        0x423652

// block 00423660
00423660  mov        eax, dword ptr [edi + 0x8856c0]
00423666  test       eax, eax
00423668  je         0x4236af

// block 0042366a
0042366a  lea        ebx, [ebx]

// block 00423670
00423670  mov        ebp, dword ptr [eax]
00423672  cmp        dword ptr [ebp], ecx
00423675  je         0x423680

// block 00423677
00423677  mov        eax, dword ptr [eax + 4]
0042367a  test       eax, eax
0042367c  jne        0x423670

// block 0042367e
0042367e  jmp        0x4236af

// block 00423680
00423680  mov        eax, ebp
00423682  test       eax, eax
00423684  je         0x4236af

// block 00423686
00423686  or         dword ptr [eax + 0x47c], esi
0042368c  cmp        dword ptr [eax + 0x18], 0
00423690  jne        0x4236af

// block 00423692
00423692  mov        eax, dword ptr [eax + 0x14]
00423695  test       eax, eax
00423697  je         0x4236af

// block 00423699
00423699  lea        esp, [esp]

// block 004236a0
004236a0  mov        ecx, dword ptr [eax]
004236a2  or         dword ptr [ecx + 0x47c], esi
004236a8  mov        eax, dword ptr [eax + 4]
004236ab  test       eax, eax
004236ad  jne        0x4236a0

// block 004236af
004236af  mov        dword ptr [edx], 0
004236b5  add        edx, 4
004236b8  sub        ebx, 1
004236bb  jne        0x423642

// block 004236bd
004236bd  cmp        byte ptr [0x4cead2], 2
004236c4  pop        edi
004236c5  pop        esi
004236c6  pop        ebp
004236c7  pop        ebx
004236c8  jne        0x4236d4

// block 004236ca
004236ca  mov        edx, dword ptr [esp + 4]
004236ce  push       edx
004236cf  call       0x424cc0

// block 004236d4
004236d4  mov        eax, dword ptr [esp + 4]
004236d8  call       0x4236f0

// block 004236dd
004236dd  mov        dword ptr [0x4b44ec], 0
004236e7  ret        4
