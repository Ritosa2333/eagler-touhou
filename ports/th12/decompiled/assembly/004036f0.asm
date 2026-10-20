
// block 004036f0
004036f0  push       ebp
004036f1  mov        ebp, esp
004036f3  and        esp, 0xfffffff8
004036f6  sub        esp, 0x24
004036f9  push       ebx
004036fa  mov        ebx, dword ptr [ebp + 8]
004036fd  mov        eax, dword ptr [ebx + 0x35bc]
00403703  push       esi
00403704  push       edi
00403705  test       al, 8
00403707  je         0x403717

// block 00403709
00403709  mov        eax, 1
0040370e  pop        edi
0040370f  pop        esi
00403710  pop        ebx
00403711  mov        esp, ebp
00403713  pop        ebp
00403714  ret        4

// block 00403717
00403717  test       al, 4
00403719  je         0x403728

// block 0040371b
0040371b  cmp        dword ptr [ebx + 0x35c4], 0x3c
00403722  jge        0x4038f9

// block 00403728
00403728  mov        esi, dword ptr [0x4ce8cc]
0040372e  call       0x45a3c0

// block 00403733
00403733  fld        dword ptr [0x4cee04]
00403739  fstp       dword ptr [ebx + 0x36f4]
0040373f  lea        esi, [ebx + 0x360c]
00403745  fld        dword ptr [0x4cee08]
0040374b  mov        ecx, 0x46
00403750  fstp       dword ptr [ebx + 0x36f8]
00403756  mov        edi, 0x4ced1c

// block 0040375b
0040375b  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0040375d
0040375d  mov        edi, 0x4ced1c
00403762  mov        dword ptr [0x4cee34], 0x4ced1c
0040376c  call       0x430a70

// block 00403771
00403771  mov        edx, dword ptr [0x4cee34]
00403777  mov        eax, dword ptr [0x4ce8f0]
0040377c  mov        ecx, dword ptr [eax]
0040377e  add        edx, 0xcc
00403784  push       edx
00403785  push       eax
00403786  mov        eax, dword ptr [ecx + 0xbc]
0040378c  call       eax

// block 0040378e
0040378e  mov        esi, dword ptr [0x4ce8cc]
00403794  mov        dword ptr [0x4cee38], 2
0040379e  call       0x45a3c0

// block 004037a3
004037a3  mov        eax, dword ptr [0x4ce8f0]
004037a8  mov        ecx, dword ptr [eax]
004037aa  mov        edx, dword ptr [ecx + 0xe4]
004037b0  push       1
004037b2  push       0xe
004037b4  push       eax
004037b5  call       edx

// block 004037b7
004037b7  mov        esi, dword ptr [0x4ce8cc]
004037bd  call       0x45a3c0

// block 004037c2
004037c2  mov        eax, dword ptr [0x4ce8f0]
004037c7  mov        ecx, dword ptr [eax]
004037c9  mov        edx, dword ptr [ecx + 0xe4]
004037cf  push       4
004037d1  push       0x17
004037d3  push       eax
004037d4  call       edx

// block 004037d6
004037d6  mov        esi, dword ptr [0x4ce8cc]
004037dc  mov        edi, dword ptr [ebx + 0x3720]
004037e2  call       0x45a3c0

// block 004037e7
004037e7  mov        eax, dword ptr [0x4ce8f0]
004037ec  mov        ecx, dword ptr [eax]
004037ee  mov        edx, dword ptr [ecx + 0xe4]
004037f4  push       edi
004037f5  push       0x22
004037f7  push       eax
004037f8  call       edx

// block 004037fa
004037fa  mov        esi, dword ptr [0x4ce8cc]
00403800  mov        edi, dword ptr [ebx + 0x3708]
00403806  call       0x45a3c0

// block 0040380b
0040380b  mov        eax, dword ptr [0x4ce8f0]
00403810  mov        ecx, dword ptr [eax]
00403812  mov        edx, dword ptr [ecx + 0xe4]
00403818  push       edi
00403819  push       0x24
0040381b  push       eax
0040381c  call       edx

// block 0040381e
0040381e  mov        esi, dword ptr [0x4ce8cc]
00403824  mov        edi, dword ptr [ebx + 0x370c]
0040382a  call       0x45a3c0

// block 0040382f
0040382f  mov        eax, dword ptr [0x4ce8f0]
00403834  mov        ecx, dword ptr [eax]
00403836  mov        edx, dword ptr [ecx + 0xe4]
0040383c  push       edi
0040383d  push       0x25
0040383f  push       eax
00403840  call       edx

// block 00403842
00403842  test       byte ptr [ebx + 0x35bc], 4
00403849  je         0x4038a7

// block 0040384b
0040384b  cmp        dword ptr [ebx + 0x35d8], 0x22
00403852  jge        0x4038a7

// block 00403854
00403854  mov        eax, dword ptr [0x4cede8]
00403859  fld1       
0040385b  mov        edx, dword ptr [0x4cedf0]
00403861  mov        ecx, dword ptr [0x4cedec]
00403867  add        edx, eax
00403869  mov        dword ptr [esp + 0x10], eax
0040386d  mov        eax, dword ptr [0x4cedf4]
00403872  add        eax, ecx
00403874  mov        dword ptr [esp + 0x1c], eax
00403878  mov        eax, dword ptr [0x4ce8f0]
0040387d  push       0
0040387f  mov        dword ptr [esp + 0x1c], edx
00403883  mov        dword ptr [esp + 0x18], ecx
00403887  mov        ecx, dword ptr [eax]
00403889  push       ecx
0040388a  fstp       dword ptr [esp]
0040388d  push       0
0040388f  push       3
00403891  lea        edx, [esp + 0x20]
00403895  push       edx
00403896  mov        edi, 1
0040389b  push       edi
0040389c  push       eax
0040389d  mov        eax, dword ptr [ecx + 0xac]
004038a3  call       eax

// block 004038a5
004038a5  jmp        0x4038fe

// block 004038a7
004038a7  mov        eax, dword ptr [0x4cede8]
004038ac  fld1       
004038ae  mov        edx, dword ptr [0x4cedf0]
004038b4  mov        ecx, dword ptr [0x4cedec]
004038ba  add        edx, eax
004038bc  mov        dword ptr [esp + 0x20], eax
004038c0  mov        eax, dword ptr [0x4cedf4]
004038c5  add        eax, ecx
004038c7  mov        dword ptr [esp + 0x2c], eax
004038cb  mov        eax, dword ptr [0x4ce8f0]
004038d0  push       0
004038d2  mov        dword ptr [esp + 0x2c], edx
004038d6  mov        edx, dword ptr [ebx + 0x3720]
004038dc  mov        dword ptr [esp + 0x28], ecx
004038e0  mov        ecx, dword ptr [eax]
004038e2  push       ecx
004038e3  fstp       dword ptr [esp]
004038e6  push       edx
004038e7  push       3
004038e9  lea        edx, [esp + 0x30]
004038ed  push       edx
004038ee  push       1
004038f0  push       eax
004038f1  mov        eax, dword ptr [ecx + 0xac]
004038f7  call       eax

// block 004038f9
004038f9  mov        edi, 1

// block 004038fe
004038fe  mov        eax, dword ptr [ebx + 0x35bc]
00403904  test       al, 4
00403906  je         0x403946

// block 00403908
00403908  cmp        dword ptr [ebx + 0x35c4], 0x1e
0040390f  jge        0x403936

// block 00403911
00403911  push       0xb
00403913  push       0
00403915  push       0
00403917  push       0
00403919  push       0x1e
0040391b  push       3
0040391d  call       0x4529a0

// block 00403922
00403922  or         dword ptr [ebx + 0x35bc], edi
00403928  push       edi
00403929  lea        eax, [ebx + 0x35c0]
0040392f  call       0x4067e0

// block 00403934
00403934  jmp        0x403946

// block 00403936
00403936  and        eax, 0xfffffffe
00403939  mov        byte ptr [ebx + 0x2777], 0
00403940  mov        dword ptr [ebx + 0x35bc], eax

// block 00403946
00403946  cmp        byte ptr [ebx + 0x2777], 0
0040394d  mov        esi, dword ptr [0x4ce8cc]
00403953  je         0x40396e

// block 00403955
00403955  mov        eax, dword ptr [ebx + 0x2774]
0040395b  mov        dword ptr [esi + 0x88ed50], edi
00403961  mov        dword ptr [esi + 0x88ed4c], eax
00403967  mov        byte ptr [ebx + 0x2777], 0

// block 0040396e
0040396e  xor        eax, eax
00403970  test       byte ptr [ebx + 0x35bc], 1
00403977  mov        dword ptr [ebx + 0x35b0], eax
0040397d  mov        dword ptr [ebx + 0x35b4], eax
00403983  mov        dword ptr [ebx + 0x35b8], eax
00403989  je         0x403aed

// block 0040398f
0040398f  cmp        dword ptr [ebx + 0x5c0], eax
00403995  je         0x403a66

// block 0040399b
0040399b  lea        ebx, [eax + 2]
0040399e  mov        esi, 0x4ce8e8
004039a3  call       0x406550

// block 004039a8
004039a8  mov        edi, esi
004039aa  call       0x430300

// block 004039af
004039af  mov        esi, dword ptr [0x4ce8cc]
004039b5  call       0x45a3c0

// block 004039ba
004039ba  mov        esi, dword ptr [0x4ce8cc]
004039c0  call       0x45a3c0

// block 004039c5
004039c5  mov        eax, dword ptr [0x4ce8f0]
004039ca  mov        ecx, dword ptr [eax]
004039cc  mov        edx, dword ptr [ecx + 0xe4]
004039d2  push       0
004039d4  push       0xe
004039d6  push       eax
004039d7  call       edx

// block 004039d9
004039d9  mov        esi, dword ptr [ebp + 8]
004039dc  add        esi, 0x1cc
004039e2  lea        edi, [ebx + 6]

// block 004039e5
004039e5  cmp        dword ptr [esi + 0x3f4], 0
004039ec  je         0x4039fb

// block 004039ee
004039ee  mov        eax, dword ptr [0x4ce8cc]
004039f3  push       eax
004039f4  mov        eax, esi
004039f6  call       0x45c900

// block 004039fb
004039fb  add        esi, 0x4b4
00403a01  sub        edi, 1
00403a04  jne        0x4039e5

// block 00403a06
00403a06  mov        esi, dword ptr [0x4ce8cc]
00403a0c  call       0x45a3c0

// block 00403a11
00403a11  mov        eax, dword ptr [0x4ce8f0]
00403a16  mov        ecx, dword ptr [eax]
00403a18  mov        edx, dword ptr [ecx + 0xe4]
00403a1e  push       1
00403a20  push       0xe
00403a22  push       eax
00403a23  call       edx

// block 00403a25
00403a25  mov        edi, 0x4ced1c
00403a2a  mov        dword ptr [0x4cee34], edi
00403a30  call       0x430a70

// block 00403a35
00403a35  mov        edx, dword ptr [0x4cee34]
00403a3b  mov        eax, dword ptr [0x4ce8f0]
00403a40  mov        ecx, dword ptr [eax]
00403a42  add        edx, 0xcc
00403a48  push       edx
00403a49  push       eax
00403a4a  mov        eax, dword ptr [ecx + 0xbc]
00403a50  call       eax

// block 00403a52
00403a52  mov        esi, dword ptr [0x4ce8cc]
00403a58  mov        dword ptr [0x4cee38], ebx
00403a5e  mov        ebx, dword ptr [ebp + 8]
00403a61  mov        edi, 1

// block 00403a66
00403a66  cmp        dword ptr [0x4cf278], edi
00403a6c  je         0x403a8c

// block 00403a6e
00403a6e  call       0x45a3c0

// block 00403a73
00403a73  mov        eax, dword ptr [0x4ce8f0]
00403a78  push       edi
00403a79  mov        dword ptr [0x4cf278], edi
00403a7f  mov        ecx, dword ptr [eax]
00403a81  mov        edx, dword ptr [ecx + 0xe4]
00403a87  push       0x1c
00403a89  push       eax
00403a8a  call       edx

// block 00403a8c
00403a8c  push       0
00403a8e  push       ebx
00403a8f  call       0x4046c0

// block 00403a94
00403a94  push       edi
00403a95  push       ebx
00403a96  call       0x4046c0

// block 00403a9b
00403a9b  push       2
00403a9d  push       ebx
00403a9e  call       0x4046c0

// block 00403aa3
00403aa3  push       3
00403aa5  push       ebx
00403aa6  call       0x4046c0

// block 00403aab
00403aab  push       4
00403aad  push       ebx
00403aae  call       0x4046c0

// block 00403ab3
00403ab3  push       5
00403ab5  push       ebx
00403ab6  call       0x4046c0

// block 00403abb
00403abb  push       6
00403abd  push       ebx
00403abe  call       0x4046c0

// block 00403ac3
00403ac3  push       7
00403ac5  push       ebx
00403ac6  call       0x4046c0

// block 00403acb
00403acb  mov        esi, dword ptr [0x4ce8cc]
00403ad1  call       0x45a3c0

// block 00403ad6
00403ad6  mov        eax, dword ptr [ebx + 0x2770]
00403adc  mov        esi, dword ptr [0x4ce8cc]
00403ae2  test       eax, eax
00403ae4  je         0x403aed

// block 00403ae6
00403ae6  inc        eax
00403ae7  mov        dword ptr [ebx + 0x2770], eax

// block 00403aed
00403aed  mov        dword ptr [esi + 0x88ed50], 0
00403af7  mov        dword ptr [esi + 0x88ed4c], 0x80808080
00403b01  cmp        dword ptr [ebx + 0x2778], 0
00403b08  je         0x403b1a

// block 00403b0a
00403b0a  mov        dword ptr [esi + 0x88ed50], edi
00403b10  mov        dword ptr [esi + 0x88ed4c], 0xff404040

// block 00403b1a
00403b1a  call       0x45a3c0

// block 00403b1f
00403b1f  mov        eax, dword ptr [0x4ce8f0]
00403b24  mov        ecx, dword ptr [eax]
00403b26  mov        edx, dword ptr [ecx + 0xe4]
00403b2c  push       0
00403b2e  push       0xe
00403b30  push       eax
00403b31  call       edx

// block 00403b33
00403b33  mov        esi, dword ptr [0x4ce8cc]
00403b39  call       0x45a3c0

// block 00403b3e
00403b3e  mov        eax, dword ptr [0x4ce8f0]
00403b43  mov        ecx, dword ptr [eax]
00403b45  mov        edx, dword ptr [ecx + 0xe4]
00403b4b  push       8
00403b4d  push       0x17
00403b4f  push       eax
00403b50  call       edx

// block 00403b52
00403b52  mov        eax, edi
00403b54  pop        edi
00403b55  pop        esi
00403b56  pop        ebx
00403b57  mov        esp, ebp
00403b59  pop        ebp
00403b5a  ret        4
