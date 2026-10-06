
// block 004287c0
004287c0  push       ebp
004287c1  mov        ebp, esp
004287c3  and        esp, 0xfffffff8
004287c6  sub        esp, 0x14
004287c9  push       ebx
004287ca  mov        ebx, ecx
004287cc  push       esi
004287cd  mov        esi, dword ptr [ebp + 8]
004287d0  push       edi
004287d1  lea        edi, [ebx + 0x454]
004287d7  mov        ecx, 0x7a

// block 004287dc
004287dc  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 004287de
004287de  mov        ax, word ptr [ebx + 0x478]
004287e5  mov        cx, word ptr [ebx + 0x47a]
004287ec  lea        esi, [ebx + 0x63c]
004287f2  mov        dword ptr [ebx + 0xc], 2
004287f9  mov        dword ptr [ebx + 0x10], 0
00428800  mov        word ptr [ebx + 0x450], ax
00428807  mov        word ptr [ebx + 0x452], cx
0042880e  call       0x402520

// block 00428813
00428813  movsx      edx, word ptr [ebx + 0x450]
0042881a  mov        ecx, dword ptr [0x4b44f4]
00428820  imul       edx, edx, 0xd0
00428826  mov        dword ptr [ebx + 0xae8], 0x428ac0
00428830  mov        dword ptr [ebx + 0xaec], ebx
00428836  mov        edx, dword ptr [edx + 0x4af280]
0042883c  mov        ecx, dword ptr [ecx + 0x488]
00428842  mov        eax, esi
00428844  call       0x454ee0

// block 00428849
00428849  mov        eax, dword ptr [esi + 0x494]
0042884f  test       eax, eax
00428851  je         0x42885c

// block 00428853
00428853  mov        edx, 2
00428858  mov        ecx, esi
0042885a  call       eax

// block 0042885c
0042885c  mov        edx, 2
00428861  push       esi
00428862  mov        word ptr [esi + 0x3c4], dx
00428869  call       0x455630

// block 0042886e
0042886e  mov        eax, dword ptr [esi + 0x47c]
00428874  mov        edx, dword ptr [0x4b44f4]
0042887a  and        eax, 0xffffff3f
0042887f  or         eax, 0x20
00428882  mov        dword ptr [esi + 0x47c], eax
00428888  mov        ecx, dword ptr [ebx + 0xab8]
0042888e  movsx      edi, word ptr [ebx + 0x47a]
00428895  and        ecx, 0xf0c7ffff
0042889b  or         ecx, 0xc00000
004288a1  mov        dword ptr [ebx + 0xab8], ecx
004288a7  mov        eax, dword ptr [edx + 0x488]
004288ad  lea        esi, [ebx + 0xaf0]
004288b3  add        edi, 0x35
004288b6  mov        dword ptr [esp + 0x10], eax
004288ba  call       0x402520

// block 004288bf
004288bf  mov        ecx, dword ptr [esp + 0x10]
004288c3  mov        al, 0x10
004288c5  push       edi
004288c6  push       esi
004288c7  mov        byte ptr [esi + 0x49d], al
004288cd  mov        byte ptr [esi + 0x49c], al
004288d3  call       0x454d10

// block 004288d8
004288d8  mov        eax, dword ptr [esi + 0x494]
004288de  xor        edi, edi
004288e0  cmp        eax, edi
004288e2  je         0x4288eb

// block 004288e4
004288e4  lea        edx, [edi + 2]
004288e7  mov        ecx, esi
004288e9  call       eax

// block 004288eb
004288eb  mov        ecx, 2
004288f0  push       esi
004288f1  mov        word ptr [esi + 0x3c4], cx
004288f8  call       0x455630

// block 004288fd
004288fd  fldz       
004288ff  mov        edx, dword ptr [esi + 0x47c]
00428905  and        edx, 0xffffff3f
0042890b  or         edx, 0x20
0042890e  mov        dword ptr [esi + 0x47c], edx
00428914  mov        eax, dword ptr [ebx + 0xf6c]
0042891a  and        eax, 0xf0ffffff
0042891f  or         eax, 0x800000
00428924  mov        dword ptr [ebx + 0xf6c], eax
0042892a  mov        eax, dword ptr [ebx + 0x448]
00428930  mov        esi, 0x4b2ed0
00428935  test       al, 1
00428937  jne        0x42895e

// block 00428939
00428939  or         eax, 1
0042893c  fst        dword ptr [ebx + 0x440]
00428942  mov        dword ptr [ebx + 0x43c], edi
00428948  mov        dword ptr [ebx + 0x438], 0xfff0bdc1
00428952  mov        dword ptr [ebx + 0x444], esi
00428958  mov        dword ptr [ebx + 0x448], eax

// block 0042895e
0042895e  fld        dword ptr [0x4a3e14]
00428964  mov        dword ptr [ebx + 0x43c], 0x1e
0042896e  fstp       dword ptr [ebx + 0x440]
00428974  mov        dword ptr [ebx + 0x438], 0x1d
0042897e  mov        eax, dword ptr [ebx + 0x634]
00428984  cmp        eax, edi
00428986  jl         0x428993

// block 00428988
00428988  push       ecx
00428989  fstp       dword ptr [esp]
0042898c  call       0x453e20

// block 00428991
00428991  fldz       

// block 00428993
00428993  mov        eax, dword ptr [ebx + 0x38]
00428996  test       al, 1
00428998  jne        0x4289b0

// block 0042899a
0042899a  or         eax, 1
0042899d  fst        dword ptr [ebx + 0x30]
004289a0  mov        dword ptr [ebx + 0x2c], edi
004289a3  mov        dword ptr [ebx + 0x28], 0xfff0bdc1
004289aa  mov        dword ptr [ebx + 0x34], esi
004289ad  mov        dword ptr [ebx + 0x38], eax

// block 004289b0
004289b0  or         ecx, 0xffffffff
004289b3  fst        dword ptr [ebx + 0x30]
004289b6  mov        dword ptr [ebx + 0x2c], edi
004289b9  mov        dword ptr [ebx + 0x28], ecx
004289bc  mov        eax, dword ptr [ebx + 0x4c]
004289bf  test       al, 1
004289c1  jne        0x4289d9

// block 004289c3
004289c3  or         eax, 1
004289c6  fst        dword ptr [ebx + 0x44]
004289c9  mov        dword ptr [ebx + 0x40], edi
004289cc  mov        dword ptr [ebx + 0x3c], 0xfff0bdc1
004289d3  mov        dword ptr [ebx + 0x48], esi
004289d6  mov        dword ptr [ebx + 0x4c], eax

// block 004289d9
004289d9  fst        dword ptr [ebx + 0x44]
004289dc  mov        dword ptr [ebx + 0x40], edi
004289df  mov        dword ptr [ebx + 0x3c], ecx
004289e2  fcom       dword ptr [ebx + 0x47c]
004289e8  mov        ecx, dword ptr [ebx + 0x454]
004289ee  mov        edx, dword ptr [ebx + 0x458]
004289f4  mov        eax, dword ptr [ebx + 0x45c]
004289fa  mov        dword ptr [ebx + 0x50], ecx
004289fd  mov        dword ptr [ebx + 0x54], edx
00428a00  mov        dword ptr [ebx + 0x58], eax
00428a03  fnstsw     ax
00428a05  test       ah, 0x44
00428a08  jnp        0x428a41

// block 00428a0a
00428a0a  fstp       st(0)
00428a0c  sub        esp, 8
00428a0f  fld        dword ptr [ebx + 0x47c]
00428a15  lea        ecx, [esp + 0x1c]
00428a19  fstp       dword ptr [esp + 4]
00428a1d  fld        dword ptr [ebx + 0x460]
00428a23  fstp       dword ptr [esp]
00428a26  call       0x42e880

// block 00428a2b
00428a2b  fld        dword ptr [esp + 0x14]
00428a2f  fadd       dword ptr [ebx + 0x50]
00428a32  fstp       dword ptr [ebx + 0x50]
00428a35  fld        dword ptr [ebx + 0x54]
00428a38  fadd       dword ptr [esp + 0x18]
00428a3c  fstp       dword ptr [ebx + 0x54]
00428a3f  fldz       

// block 00428a41
00428a41  fld        dword ptr [ebx + 0x468]
00428a47  fstp       dword ptr [esp + 0x10]
00428a4b  fld        dword ptr [esp + 0x10]
00428a4f  fst        dword ptr [ebx + 0x6c]
00428a52  fld        dword ptr [ebx + 0x470]
00428a58  fstp       dword ptr [ebx + 0x70]
00428a5b  fld        dword ptr [ebx + 0x474]
00428a61  fstp       dword ptr [esp + 0x10]
00428a65  fld        dword ptr [esp + 0x10]
00428a69  fst        dword ptr [ebx + 0x74]
00428a6c  fld        dword ptr [ebx + 0x460]
00428a72  fstp       dword ptr [esp + 0x10]
00428a76  fld        dword ptr [esp + 0x10]
00428a7a  fst        dword ptr [ebx + 0x68]
00428a7d  fxch       st(2)
00428a7f  fcomp      st(3)
00428a81  fnstsw     ax
00428a83  test       ah, 0x41
00428a86  jne        0x428a92

// block 00428a88
00428a88  fstp       st(2)
00428a8a  fld        dword ptr [0x4a4240]
00428a90  jmp        0x428a94

// block 00428a92
00428a92  fxch       st(2)

// block 00428a94
00428a94  fstp       dword ptr [ebx + 0x78]
00428a97  sub        esp, 8
00428a9a  fxch       st(1)
00428a9c  lea        ecx, [ebx + 0x5c]
00428a9f  fstp       dword ptr [esp + 4]
00428aa3  fstp       dword ptr [esp]
00428aa6  call       0x42e880

// block 00428aab
00428aab  pop        edi
00428aac  pop        esi
00428aad  xor        eax, eax
00428aaf  pop        ebx
00428ab0  mov        esp, ebp
00428ab2  pop        ebp
00428ab3  ret        4
