
// block 004830fd
004830fd  mov        edi, edi
004830ff  push       ebp
00483100  mov        ebp, esp
00483102  push       ebx
00483103  push       esi
00483104  mov        esi, dword ptr [ebp + 8]
00483107  xor        ebx, ebx
00483109  push       edi
0048310a  cmp        dword ptr [ebp + 0x14], ebx
0048310d  jne        0x48311f

// block 0048310f
0048310f  cmp        esi, ebx
00483111  jne        0x483123

// block 00483113
00483113  cmp        dword ptr [ebp + 0xc], ebx
00483116  jne        0x48312a

// block 00483118
00483118  xor        eax, eax

// block 0048311a
0048311a  pop        edi
0048311b  pop        esi
0048311c  pop        ebx
0048311d  pop        ebp
0048311e  ret        

// block 0048311f
0048311f  cmp        esi, ebx
00483121  je         0x48312a

// block 00483123
00483123  mov        edi, dword ptr [ebp + 0xc]
00483126  cmp        edi, ebx
00483128  ja         0x483145

// block 0048312a
0048312a  call       0x46e96f

// block 0048312f
0048312f  push       0x16
00483131  pop        esi
00483132  mov        dword ptr [eax], esi

// block 00483134
00483134  push       ebx
00483135  push       ebx
00483136  push       ebx
00483137  push       ebx
00483138  push       ebx
00483139  call       0x470f2d

// block 0048313e
0048313e  add        esp, 0x14
00483141  mov        eax, esi
00483143  jmp        0x48311a

// block 00483145
00483145  cmp        dword ptr [ebp + 0x14], ebx
00483148  jne        0x48314e

// block 0048314a
0048314a  mov        byte ptr [esi], bl
0048314c  jmp        0x483118

// block 0048314e
0048314e  mov        edx, dword ptr [ebp + 0x10]
00483151  cmp        edx, ebx
00483153  jne        0x483159

// block 00483155
00483155  mov        byte ptr [esi], bl
00483157  jmp        0x48312a

// block 00483159
00483159  cmp        dword ptr [ebp + 0x14], -1
0048315d  mov        eax, esi
0048315f  jne        0x483170

// block 00483161
00483161  mov        cl, byte ptr [edx]
00483163  mov        byte ptr [eax], cl
00483165  inc        eax
00483166  inc        edx
00483167  cmp        cl, bl
00483169  je         0x483189

// block 0048316b
0048316b  dec        edi
0048316c  jne        0x483161

// block 0048316e
0048316e  jmp        0x483189

// block 00483170
00483170  mov        cl, byte ptr [edx]
00483172  mov        byte ptr [eax], cl
00483174  inc        eax
00483175  inc        edx
00483176  cmp        cl, bl
00483178  je         0x483182

// block 0048317a
0048317a  dec        edi
0048317b  je         0x483182

// block 0048317d
0048317d  dec        dword ptr [ebp + 0x14]
00483180  jne        0x483170

// block 00483182
00483182  cmp        dword ptr [ebp + 0x14], ebx
00483185  jne        0x483189

// block 00483187
00483187  mov        byte ptr [eax], bl

// block 00483189
00483189  cmp        edi, ebx
0048318b  jne        0x483118

// block 0048318d
0048318d  cmp        dword ptr [ebp + 0x14], -1
00483191  jne        0x4831a2

// block 00483193
00483193  mov        eax, dword ptr [ebp + 0xc]
00483196  push       0x50
00483198  mov        byte ptr [esi + eax - 1], bl
0048319c  pop        eax
0048319d  jmp        0x48311a

// block 004831a2
004831a2  mov        byte ptr [esi], bl
004831a4  call       0x46e96f

// block 004831a9
004831a9  push       0x22
004831ab  pop        ecx
004831ac  mov        dword ptr [eax], ecx
004831ae  mov        esi, ecx
004831b0  jmp        0x483134
