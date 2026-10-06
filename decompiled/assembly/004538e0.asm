
// block 004538e0
004538e0  push       ecx
004538e1  push       ebx
004538e2  push       esi
004538e3  mov        esi, dword ptr [esp + 0x10]
004538e7  cmp        dword ptr [esi*4 + 0x4d0da8], 0
004538ef  mov        ebx, eax
004538f1  je         0x453931

// block 004538f3
004538f3  mov        eax, esi
004538f5  shl        eax, 8
004538f8  add        eax, 0x4d3654
004538fd  mov        ecx, ebx
004538ff  nop        

// block 00453900
00453900  mov        dl, byte ptr [ecx]
00453902  cmp        dl, byte ptr [eax]
00453904  jne        0x453920

// block 00453906
00453906  test       dl, dl
00453908  je         0x45391c

// block 0045390a
0045390a  mov        dl, byte ptr [ecx + 1]
0045390d  cmp        dl, byte ptr [eax + 1]
00453910  jne        0x453920

// block 00453912
00453912  add        ecx, 2
00453915  add        eax, 2
00453918  test       dl, dl
0045391a  jne        0x453900

// block 0045391c
0045391c  xor        eax, eax
0045391e  jmp        0x453925

// block 00453920
00453920  sbb        eax, eax
00453922  sbb        eax, -1

// block 00453925
00453925  test       eax, eax
00453927  jne        0x453931

// block 00453929
00453929  pop        esi
0045392a  xor        eax, eax
0045392c  pop        ebx
0045392d  pop        ecx
0045392e  ret        4

// block 00453931
00453931  mov        ecx, esi
00453933  shl        ecx, 8
00453936  lea        edx, [ecx + 0x4d3654]
0045393c  mov        eax, ebx
0045393e  sub        edx, ebx

// block 00453940
00453940  mov        cl, byte ptr [eax]
00453942  mov        byte ptr [edx + eax], cl
00453945  inc        eax
00453946  test       cl, cl
00453948  jne        0x453940

// block 0045394a
0045394a  test       byte ptr [0x4ceae8], 0x10
00453951  je         0x453929

// block 00453953
00453953  cmp        dword ptr [0x4cf4f8], 0
0045395a  je         0x453929

// block 0045395c
0045395c  mov        eax, dword ptr [esi*4 + 0x4d0da8]
00453963  test       eax, eax
00453965  je         0x45397b

// block 00453967
00453967  push       eax
00453968  call       0x46c941

// block 0045396d
0045396d  add        esp, 4
00453970  mov        dword ptr [esi*4 + 0x4d0da8], 0

// block 0045397b
0045397b  push       edi
0045397c  push       esi
0045397d  push       0x4a2f10
00453982  call       0x454b00

// block 00453987
00453987  mov        edi, 0x8000
0045398c  add        esp, 8
0045398f  test       dword ptr [0x4cee78], edi
00453995  je         0x4539a8

// block 00453997
00453997  push       0x4cf128
0045399c  call       dword ptr [0x498088]

// block 004539a2
004539a2  inc        byte ptr [0x4cf21a]

// block 004539a8
004539a8  push       0
004539aa  push       0x8000080
004539af  push       3
004539b1  push       0
004539b3  push       1
004539b5  push       0x80000000
004539ba  push       0x4d4654
004539bf  call       dword ptr [0x49809c]

// block 004539c5
004539c5  mov        esi, eax
004539c7  cmp        esi, -1
004539ca  jne        0x453a01

// block 004539cc
004539cc  push       0x4d4654
004539d1  push       0x4a2f2c
004539d6  call       0x454b00

// block 004539db
004539db  add        esp, 8
004539de  test       dword ptr [0x4cee78], edi
004539e4  je         0x4539f7

// block 004539e6
004539e6  push       0x4cf128
004539eb  call       dword ptr [0x49808c]

// block 004539f1
004539f1  dec        byte ptr [0x4cf21a]

// block 004539f7
004539f7  pop        edi
004539f8  pop        esi
004539f9  or         eax, 0xffffffff
004539fc  pop        ebx
004539fd  pop        ecx
004539fe  ret        4

// block 00453a01
00453a01  push       ebp
00453a02  push       0x4cf4e8
00453a07  mov        ecx, ebx
00453a09  call       0x453670

// block 00453a0e
00453a0e  mov        edx, dword ptr [0x4d0e6c]
00453a14  mov        ebx, eax
00453a16  imul       ebx, ebx, 0x34
00453a19  mov        eax, dword ptr [ebx + edx + 0x10]
00453a1d  push       0
00453a1f  push       0
00453a21  push       eax
00453a22  push       esi
00453a23  call       dword ptr [0x4980a0]

// block 00453a29
00453a29  mov        ecx, dword ptr [0x4d0e6c]
00453a2f  mov        eax, dword ptr [ebx + ecx + 0x14]
00453a33  push       eax
00453a34  call       0x46d04a

// block 00453a39
00453a39  mov        ebp, eax
00453a3b  add        esp, 4
00453a3e  test       ebp, ebp
00453a40  jne        0x453a73

// block 00453a42
00453a42  push       esi
00453a43  call       dword ptr [0x4980a4]

// block 00453a49
00453a49  push       0x4d4654
00453a4e  push       0x4a2f2c
00453a53  call       0x454b00

// block 00453a58
00453a58  add        esp, 8
00453a5b  lea        esi, [ebp + 2]
00453a5e  mov        edi, 0x4ce8e8
00453a63  call       0x431800

// block 00453a68
00453a68  pop        ebp
00453a69  pop        edi
00453a6a  pop        esi
00453a6b  or         eax, 0xffffffff
00453a6e  pop        ebx
00453a6f  pop        ecx
00453a70  ret        4

// block 00453a73
00453a73  mov        eax, dword ptr [0x4d0e6c]
00453a78  mov        ecx, dword ptr [ebx + eax + 0x14]
00453a7c  push       0
00453a7e  lea        edx, [esp + 0x14]
00453a82  push       edx
00453a83  push       ecx
00453a84  push       ebp
00453a85  push       esi
00453a86  call       dword ptr [0x4980a8]

// block 00453a8c
00453a8c  push       esi
00453a8d  call       dword ptr [0x4980a4]

// block 00453a93
00453a93  mov        esi, 2
00453a98  mov        edi, 0x4ce8e8
00453a9d  call       0x431800

// block 00453aa2
00453aa2  mov        edx, dword ptr [0x4d0e6c]
00453aa8  mov        eax, dword ptr [esp + 0x18]
00453aac  add        ebx, edx
00453aae  mov        dword ptr [eax*4 + 0x4d0da8], ebp
00453ab5  mov        dword ptr [eax*4 + 0x4d0de8], ebp
00453abc  pop        ebp
00453abd  mov        dword ptr [eax*4 + 0x4d0d68], ebx
00453ac4  mov        ecx, ebx
00453ac6  mov        edx, dword ptr [ecx + 0x14]
00453ac9  pop        edi
00453aca  pop        esi
00453acb  mov        dword ptr [eax*4 + 0x4d0e28], edx
00453ad2  xor        eax, eax
00453ad4  pop        ebx
00453ad5  pop        ecx
00453ad6  ret        4
