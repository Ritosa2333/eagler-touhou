
// block 0044e130
0044e130  sub        esp, 0x40
0044e133  push       ebx
0044e134  xor        ebx, ebx
0044e136  cmp        dword ptr [esi + 0x11c], ebx
0044e13c  jne        0x44e147

// block 0044e13e
0044e13e  xor        al, al
0044e140  pop        ebx
0044e141  add        esp, 0x40
0044e144  ret        4

// block 0044e147
0044e147  push       edi
0044e148  mov        edi, dword ptr [esp + 0x4c]
0044e14c  mov        eax, dword ptr [edi]
0044e14e  mov        edx, dword ptr [eax + 0x30]
0044e151  lea        ecx, [esp + 0x28]
0044e155  push       ecx
0044e156  push       edi
0044e157  call       edx

// block 0044e159
0044e159  mov        eax, dword ptr [esi + 0x104]
0044e15f  mov        ecx, dword ptr [esi + 0x108]
0044e165  mov        edx, dword ptr [edi]
0044e167  mov        edx, dword ptr [edx + 0x34]
0044e16a  push       ebx
0044e16b  mov        dword ptr [esp + 0x24], eax
0044e16f  lea        eax, [esp + 0x1c]
0044e173  mov        dword ptr [esp + 0x28], ecx
0044e177  push       eax
0044e178  lea        ecx, [esp + 0x18]
0044e17c  push       ecx
0044e17d  push       edi
0044e17e  mov        dword ptr [esp + 0x28], ebx
0044e182  mov        dword ptr [esp + 0x2c], ebx
0044e186  call       edx

// block 0044e188
0044e188  test       eax, eax
0044e18a  je         0x44e196

// block 0044e18c
0044e18c  pop        edi
0044e18d  xor        al, al
0044e18f  pop        ebx
0044e190  add        esp, 0x40
0044e193  ret        4

// block 0044e196
0044e196  mov        ecx, dword ptr [esp + 0x28]
0044e19a  mov        eax, dword ptr [esp + 0x10]
0044e19e  mov        edi, dword ptr [esi + 0x120]
0044e1a4  mov        ebx, dword ptr [esp + 0x14]
0044e1a8  push       ebp
0044e1a9  mov        ebp, dword ptr [esi + 0x110]
0044e1af  mov        dword ptr [esp + 0x10], eax
0044e1b3  cmp        ecx, dword ptr [esi + 0x100]
0044e1b9  jne        0x44e1f2

// block 0044e1bb
0044e1bb  cmp        dword ptr [esi + 0x108], 0
0044e1c2  mov        dword ptr [esp + 0xc], 0
0044e1ca  jle        0x44e1f2

// block 0044e1cc
0044e1cc  lea        esp, [esp]

// block 0044e1d0
0044e1d0  push       ebp
0044e1d1  push       edi
0044e1d2  push       ebx
0044e1d3  call       0x47a330

// block 0044e1d8
0044e1d8  mov        eax, dword ptr [esp + 0x18]
0044e1dc  add        ebx, dword ptr [esp + 0x1c]
0044e1e0  inc        eax
0044e1e1  add        esp, 0xc
0044e1e4  add        edi, ebp
0044e1e6  cmp        eax, dword ptr [esi + 0x108]
0044e1ec  mov        dword ptr [esp + 0xc], eax
0044e1f0  jl         0x44e1d0

// block 0044e1f2
0044e1f2  mov        eax, dword ptr [esp + 0x50]
0044e1f6  mov        edx, dword ptr [eax]
0044e1f8  push       eax
0044e1f9  mov        eax, dword ptr [edx + 0x38]
0044e1fc  call       eax

// block 0044e1fe
0044e1fe  pop        ebp
0044e1ff  pop        edi
0044e200  mov        al, 1
0044e202  pop        ebx
0044e203  add        esp, 0x40
0044e206  ret        4
