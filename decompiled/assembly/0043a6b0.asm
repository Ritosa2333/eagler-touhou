
// block 0043a6b0
0043a6b0  sub        esp, 0xc
0043a6b3  mov        ecx, dword ptr [0x4b4514]
0043a6b9  push       esi
0043a6ba  mov        esi, edx
0043a6bc  mov        eax, dword ptr [esi + 0x74]
0043a6bf  movsx      edx, byte ptr [eax + 0x1c]
0043a6c3  imul       edx, edx, 0xe4
0043a6c9  fild       dword ptr [edx + ecx + 0x81d8]
0043a6d0  fld        qword ptr [0x4a3d40]
0043a6d6  fmul       st(1), st(0)
0043a6d8  fxch       st(1)
0043a6da  fstp       dword ptr [esp + 4]
0043a6de  cmp        dword ptr [esi + 0x48], 2
0043a6e2  fimul      dword ptr [edx + ecx + 0x81dc]
0043a6e9  lea        eax, [edx + ecx + 0x81d8]
0043a6f0  mov        eax, dword ptr [esp + 4]
0043a6f4  mov        dword ptr [esi + 0x14], eax
0043a6f7  fstp       dword ptr [esp + 8]
0043a6fb  mov        edx, dword ptr [esp + 8]
0043a6ff  fldz       
0043a701  mov        dword ptr [esi + 0x18], edx
0043a704  fstp       dword ptr [esp + 0xc]
0043a708  mov        eax, dword ptr [esp + 0xc]
0043a70c  mov        dword ptr [esi + 0x1c], eax
0043a70f  je         0x43a7f4

// block 0043a715
0043a715  fld        dword ptr [ecx + 0x97c]
0043a71b  push       ebx
0043a71c  fmul       qword ptr [0x4a3d30]
0043a722  push       edi
0043a723  mov        edi, dword ptr [0x4d1068]
0043a729  mov        ebx, dword ptr [edi]
0043a72b  fdiv       qword ptr [0x4a3d28]
0043a731  call       0x4931e0

// block 0043a736
0043a736  mov        ecx, dword ptr [ebx + 0x40]
0043a739  push       eax
0043a73a  push       edi
0043a73b  call       ecx

// block 0043a73d
0043a73d  fld        dword ptr [esi + 0x6c]
0043a740  fcomp      qword ptr [0x4a3d50]
0043a746  fnstsw     ax
0043a748  test       ah, 5
0043a74b  jp         0x43a759

// block 0043a74d
0043a74d  fld        dword ptr [esi + 0x6c]
0043a750  fadd       qword ptr [0x4a4140]
0043a756  fstp       dword ptr [esi + 0x6c]

// block 0043a759
0043a759  mov        ebx, 1
0043a75e  xor        edi, edi
0043a760  cmp        dword ptr [esi + 0x48], ebx
0043a763  jne        0x43a7d2

// block 0043a765
0043a765  mov        eax, dword ptr [0x4b4514]
0043a76a  cmp        dword ptr [eax + 0xc424], edi
0043a770  jl         0x43a79c

// block 0043a772
0043a772  mov        edx, dword ptr [esi + 0x74]
0043a775  movsx      ecx, byte ptr [edx + 0x1c]
0043a779  sub        ecx, ebx
0043a77b  cmp        ecx, dword ptr [eax + 0xc41c]
0043a781  jge        0x43a79c

// block 0043a783
0043a783  mov        eax, dword ptr [0x4b43e4]
0043a788  cmp        eax, edi
0043a78a  je         0x43a794

// block 0043a78c
0043a78c  cmp        dword ptr [eax + 0x6d30], edi
0043a792  jne        0x43a79c

// block 0043a794
0043a794  cmp        dword ptr [0x4b43dc], edi
0043a79a  jne        0x43a7d2

// block 0043a79c
0043a79c  mov        edx, dword ptr [esi + 0x4c]
0043a79f  push       edx
0043a7a0  mov        dword ptr [esi + 0x48], 2
0043a7a7  call       0x461970

// block 0043a7ac
0043a7ac  mov        eax, dword ptr [esi + 0x50]
0043a7af  push       eax
0043a7b0  call       0x461970

// block 0043a7b5
0043a7b5  mov        ecx, dword ptr [esi + 0x74]
0043a7b8  movsx      edx, byte ptr [ecx + 0x1c]
0043a7bc  mov        eax, dword ptr [0x4b4514]
0043a7c1  mov        dword ptr [eax + edx*4 + 0xc430], edi
0043a7c8  mov        eax, 0x15
0043a7cd  call       0x453f10

// block 0043a7d2
0043a7d2  cmp        dword ptr [esi + 0x58], edi
0043a7d5  jne        0x43a7f2

// block 0043a7d7
0043a7d7  cmp        dword ptr [esi + 0x48], ebx
0043a7da  jne        0x43a7f2

// block 0043a7dc
0043a7dc  cmp        dword ptr [esi + 0x5c], ebx
0043a7df  jne        0x43a7f2

// block 0043a7e1
0043a7e1  mov        ecx, dword ptr [esi + 0x4c]
0043a7e4  push       ecx
0043a7e5  mov        ebx, 3
0043a7ea  call       0x461970

// block 0043a7ef
0043a7ef  mov        dword ptr [esi + 0x5c], edi

// block 0043a7f2
0043a7f2  pop        edi
0043a7f3  pop        ebx

// block 0043a7f4
0043a7f4  xor        eax, eax
0043a7f6  pop        esi
0043a7f7  add        esp, 0xc
0043a7fa  ret        
