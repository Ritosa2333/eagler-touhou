
// block 00493064
00493064  mov        edi, edi
00493066  push       ebp
00493067  mov        ebp, esp
00493069  push       ebx
0049306a  push       esi
0049306b  push       edi
0049306c  call       0x475467

// block 00493071
00493071  cmp        dword ptr [eax + 0x20c], 0
00493078  mov        eax, dword ptr [ebp + 0x18]
0049307b  mov        ecx, dword ptr [ebp + 8]
0049307e  mov        edi, 0xe06d7363
00493083  mov        esi, 0x1fffffff
00493088  mov        ebx, 0x19930522
0049308d  jne        0x4930af

// block 0049308f
0049308f  mov        edx, dword ptr [ecx]
00493091  cmp        edx, edi
00493093  je         0x4930af

// block 00493095
00493095  cmp        edx, 0x80000026
0049309b  je         0x4930af

// block 0049309d
0049309d  mov        edx, dword ptr [eax]
0049309f  and        edx, esi
004930a1  cmp        edx, ebx
004930a3  jb         0x4930af

// block 004930a5
004930a5  test       byte ptr [eax + 0x20], 1
004930a9  jne        0x493142

// block 004930af
004930af  test       byte ptr [ecx + 4], 0x66
004930b3  je         0x4930d8

// block 004930b5
004930b5  cmp        dword ptr [eax + 4], 0
004930b9  je         0x493142

// block 004930bf
004930bf  cmp        dword ptr [ebp + 0x1c], 0
004930c3  jne        0x493142

// block 004930c5
004930c5  push       -1
004930c7  push       eax
004930c8  push       dword ptr [ebp + 0x14]
004930cb  push       dword ptr [ebp + 0xc]
004930ce  call       0x49205b

// block 004930d3
004930d3  add        esp, 0x10
004930d6  jmp        0x493142

// block 004930d8
004930d8  cmp        dword ptr [eax + 0xc], 0
004930dc  jne        0x4930f0

// block 004930de
004930de  mov        edx, dword ptr [eax]
004930e0  and        edx, esi
004930e2  cmp        edx, 0x19930521
004930e8  jb         0x493142

// block 004930ea
004930ea  cmp        dword ptr [eax + 0x1c], 0
004930ee  je         0x493142

// block 004930f0
004930f0  cmp        dword ptr [ecx], edi
004930f2  jne        0x493126

// block 004930f4
004930f4  cmp        dword ptr [ecx + 0x10], 3
004930f8  jb         0x493126

// block 004930fa
004930fa  cmp        dword ptr [ecx + 0x14], ebx
004930fd  jbe        0x493126

// block 004930ff
004930ff  mov        edx, dword ptr [ecx + 0x1c]
00493102  mov        edx, dword ptr [edx + 8]
00493105  test       edx, edx
00493107  je         0x493126

// block 00493109
00493109  movzx      esi, byte ptr [ebp + 0x24]
0049310d  push       esi
0049310e  push       dword ptr [ebp + 0x20]
00493111  push       dword ptr [ebp + 0x1c]
00493114  push       eax
00493115  push       dword ptr [ebp + 0x14]
00493118  push       dword ptr [ebp + 0x10]
0049311b  push       dword ptr [ebp + 0xc]
0049311e  push       ecx
0049311f  call       edx

// block 00493121
00493121  add        esp, 0x20
00493124  jmp        0x493145

// block 00493126
00493126  push       dword ptr [ebp + 0x20]
00493129  push       dword ptr [ebp + 0x1c]
0049312c  push       dword ptr [ebp + 0x24]
0049312f  push       eax
00493130  push       dword ptr [ebp + 0x14]
00493133  push       dword ptr [ebp + 0x10]
00493136  push       dword ptr [ebp + 0xc]
00493139  push       ecx
0049313a  call       0x492d00

// block 0049313f
0049313f  add        esp, 0x20

// block 00493142
00493142  xor        eax, eax
00493144  inc        eax

// block 00493145
00493145  pop        edi
00493146  pop        esi
00493147  pop        ebx
00493148  pop        ebp
00493149  ret        
