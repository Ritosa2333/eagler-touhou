
// block 00485a06
00485a06  mov        edi, edi
00485a08  push       ebp
00485a09  mov        ebp, esp
00485a0b  xor        eax, eax
00485a0d  push       ebx
00485a0e  xor        ebx, ebx
00485a10  inc        eax
00485a11  cmp        dword ptr [ebp + 0xc], ebx
00485a14  jl         0x485a5c

// block 00485a16
00485a16  push       esi
00485a17  push       edi

// block 00485a18
00485a18  test       eax, eax
00485a1a  je         0x485a5a

// block 00485a1c
00485a1c  mov        eax, dword ptr [ebp + 0xc]
00485a1f  add        eax, ebx
00485a21  cdq        
00485a22  sub        eax, edx
00485a24  mov        esi, eax
00485a26  mov        eax, dword ptr [ebp + 8]
00485a29  sar        esi, 1
00485a2b  lea        edi, [eax + esi*8]
00485a2e  push       dword ptr [edi]
00485a30  mov        eax, dword ptr [ebp + 0x10]
00485a33  push       dword ptr [eax]
00485a35  call       0x46e3f8

// block 00485a3a
00485a3a  pop        ecx
00485a3b  pop        ecx
00485a3c  test       eax, eax
00485a3e  jne        0x485a4a

// block 00485a40
00485a40  mov        ecx, dword ptr [ebp + 0x10]
00485a43  add        edi, 4
00485a46  mov        dword ptr [ecx], edi
00485a48  jmp        0x485a55

// block 00485a4a
00485a4a  jge        0x485a52

// block 00485a4c
00485a4c  dec        esi
00485a4d  mov        dword ptr [ebp + 0xc], esi
00485a50  jmp        0x485a55

// block 00485a52
00485a52  lea        ebx, [esi + 1]

// block 00485a55
00485a55  cmp        ebx, dword ptr [ebp + 0xc]
00485a58  jle        0x485a18

// block 00485a5a
00485a5a  pop        edi
00485a5b  pop        esi

// block 00485a5c
00485a5c  xor        ecx, ecx
00485a5e  test       eax, eax
00485a60  sete       cl
00485a63  pop        ebx
00485a64  mov        eax, ecx
00485a66  pop        ebp
00485a67  ret        
