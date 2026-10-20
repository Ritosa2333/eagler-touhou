
// block 004391c0
004391c0  sub        esp, 0x10
004391c3  cmp        dword ptr [0x4b0cdc], 0x5f5e0ff
004391cd  push       ebx
004391ce  push       ebp
004391cf  mov        ebp, dword ptr [esp + 0x1c]
004391d3  push       esi
004391d4  push       edi
004391d5  mov        ebx, 1
004391da  jge        0x4391e2

// block 004391dc
004391dc  add        dword ptr [0x4b0cdc], ebx

// block 004391e2
004391e2  mov        eax, dword ptr [0x4b0c78]
004391e7  mov        ecx, dword ptr [0x4b0cf4]
004391ed  add        eax, 0x64
004391f0  cmp        eax, ecx
004391f2  mov        dword ptr [0x4b0c78], eax
004391f7  jle        0x4391ff

// block 004391f9
004391f9  mov        dword ptr [0x4b0c78], ecx

// block 004391ff
004391ff  test       dword ptr [0x4cee78], 0x8000
00439209  mov        eax, dword ptr [0x4b43c8]
0043920e  mov        edi, dword ptr [eax + 0x4debdc]
00439214  je         0x439227

// block 00439216
00439216  push       0x4cf1d0
0043921b  call       dword ptr [0x498088]

// block 00439221
00439221  add        byte ptr [0x4cf221], bl

// block 00439227
00439227  add        dword ptr [edi + 0x130], ebx
0043922d  call       0x4621c0

// block 00439232
00439232  mov        esi, eax
00439234  or         dword ptr [esi + 0x480], ebx
0043923a  mov        dword ptr [esi + 0x20], 0x17
00439241  test       ebp, ebp
00439243  jne        0x439273

// block 00439245
00439245  fldz       
00439247  fst        dword ptr [esp + 0x14]
0043924b  mov        ecx, dword ptr [esp + 0x14]
0043924f  fst        dword ptr [esp + 0x18]
00439253  mov        edx, dword ptr [esp + 0x18]
00439257  fstp       dword ptr [esp + 0x1c]
0043925b  mov        eax, dword ptr [esp + 0x1c]
0043925f  mov        dword ptr [esi + 0x430], ecx
00439265  mov        dword ptr [esi + 0x434], edx
0043926b  mov        dword ptr [esi + 0x438], eax
00439271  jmp        0x4392a0

// block 00439273
00439273  fld        dword ptr [ebp]
00439276  fadd       qword ptr [0x4a3d70]
0043927c  fadd       qword ptr [0x4a3d28]
00439282  fstp       dword ptr [esi + 0x430]
00439288  fld        dword ptr [ebp + 4]
0043928b  fadd       qword ptr [0x4a3d68]
00439291  fstp       dword ptr [esi + 0x434]
00439297  fld        dword ptr [ebp + 8]
0043929a  fstp       dword ptr [esi + 0x438]

// block 004392a0
004392a0  push       0x90
004392a5  push       esi
004392a6  mov        ecx, edi
004392a8  call       0x454d10

// block 004392ad
004392ad  mov        ebx, esi
004392af  lea        eax, [esp + 0x24]
004392b3  call       0x461250

// block 004392b8
004392b8  test       dword ptr [0x4cee78], 0x8000
004392c2  je         0x4392d5

// block 004392c4
004392c4  push       0x4cf1d0
004392c9  call       dword ptr [0x49808c]

// block 004392cf
004392cf  dec        byte ptr [0x4cf221]

// block 004392d5
004392d5  fld        dword ptr [ebp]
004392d8  push       ecx
004392d9  mov        eax, 0x2c
004392de  fstp       dword ptr [esp]
004392e1  call       0x453e20

// block 004392e6
004392e6  pop        edi
004392e7  pop        esi
004392e8  pop        ebp
004392e9  pop        ebx
004392ea  add        esp, 0x10
004392ed  ret        4
