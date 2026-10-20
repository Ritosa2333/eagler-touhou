
// block 00454078
00454078  test       byte ptr [0x4ceae8], 0x10
0045407f  je         0x4540fd

// block 00454081
00454081  mov        edi, dword ptr [ebp + 4]
00454084  cmp        edi, ebx
00454086  jl         0x4540fd

// block 00454088
00454088  mov        eax, dword ptr [ebp + 8]
0045408b  cmp        eax, ebx
0045408d  jne        0x4540a1

// block 0045408f
0045408f  call       0x453ae0

// block 00454094
00454094  test       eax, eax
00454096  je         0x4543a1

// block 0045409c
0045409c  jmp        0x454282

// block 004540a1
004540a1  cmp        eax, 2
004540a4  jne        0x4540be

// block 004540a6
004540a6  mov        edi, dword ptr [0x4d4754]
004540ac  cmp        edi, ebx
004540ae  je         0x4543a1

// block 004540b4
004540b4  call       0x4669f0

// block 004540b9
004540b9  jmp        0x454161

// block 004540be
004540be  cmp        eax, 5
004540c1  jne        0x4540e6

// block 004540c3
004540c3  mov        ecx, dword ptr [0x4d4754]
004540c9  call       0x4662f0

// block 004540ce
004540ce  mov        ecx, dword ptr [ecx + 0xc]
004540d1  mov        edx, dword ptr [ecx + 0x90]
004540d7  xor        ecx, ecx
004540d9  cmp        dword ptr [edx + 0x1c], ecx
004540dc  setne      cl
004540df  push       ecx
004540e0  mov        dword ptr [ebp + 4], ecx
004540e3  push       eax
004540e4  jmp        0x454156

// block 004540e6
004540e6  cmp        eax, 7
004540e9  je         0x4542c1

// block 004540ef
004540ef  cmp        eax, 0x14
004540f2  jl         0x4543a1

// block 004540f8
004540f8  jmp        0x454282

// block 004540fd
004540fd  mov        ecx, dword ptr [0x4d4754]
00454103  cmp        ecx, ebx
00454105  je         0x454282

// block 0045410b
0045410b  mov        eax, dword ptr [ebp + 8]
0045410e  cmp        eax, ebx
00454110  je         0x4542d0

// block 00454116
00454116  cmp        eax, 1
00454119  je         0x4542d8

// block 0045411f
0045411f  cmp        eax, 2
00454122  je         0x4542ed

// block 00454128
00454128  cmp        eax, 3
0045412b  jne        0x45416e

// block 0045412d
0045412d  call       0x4662f0

// block 00454132
00454132  mov        edi, ecx
00454134  mov        ebx, eax
00454136  call       0x4669f0

// block 0045413b
0045413b  mov        eax, dword ptr [0x4d4754]
00454140  mov        ecx, dword ptr [eax + 0xc]
00454143  mov        edx, dword ptr [ecx + 0x90]
00454149  xor        eax, eax
0045414b  cmp        dword ptr [edx + 0x1c], eax
0045414e  setne      al
00454151  push       eax
00454152  mov        dword ptr [ebp + 4], eax

// block 00454156
00454156  mov        ebx, dword ptr [0x4d4754]
0045415c  call       0x465fe0

// block 00454161
00454161  test       eax, eax
00454163  jge        0x4543a1

// block 00454169
00454169  jmp        0x454282

// block 0045416e
0045416e  cmp        eax, 4
00454171  je         0x454327

// block 00454177
00454177  cmp        eax, 7
0045417a  jl         0x4543a1

// block 00454180
00454180  jmp        0x454282

// block 004542c1
004542c1  mov        eax, dword ptr [0x4d4754]
004542c6  call       0x466350

// block 004542cb
004542cb  jmp        0x4543a1

// block 004542d8
004542d8  cmp        dword ptr [ecx + 0x78], ebx
004542db  jne        0x4543a4

// block 004542e1
004542e1  mov        esi, ecx
004542e3  call       0x465de0

// block 004542e8
004542e8  jmp        0x4543a1

// block 004542ed
004542ed  mov        eax, dword ptr [ebp + 4]
004542f0  cmp        eax, ebx
004542f2  jl         0x4542fe

// block 004542f4
004542f4  shl        eax, 8
004542f7  add        eax, 0x4d3654
004542fc  jmp        0x454301

// block 004542fe
004542fe  lea        eax, [ebp + 0xc]

// block 00454301
00454301  push       0x4cf4e8
00454306  mov        ecx, eax
00454308  call       0x453670

// block 0045430d
0045430d  imul       eax, eax, 0x34
00454310  mov        ecx, dword ptr [0x4d4754]
00454316  add        eax, dword ptr [0x4d0e6c]
0045431c  mov        ecx, dword ptr [ecx + 0xc]
0045431f  push       ebx
00454320  call       0x466bc0

// block 00454325
00454325  jmp        0x4543a1

// block 00454327
00454327  mov        eax, ecx
00454329  call       0x466350

// block 0045432e
0045432e  jmp        0x4543a1
