
// block 004056e0
004056e0  sub        esp, 0x14
004056e3  push       ebx
004056e4  push       ebp
004056e5  mov        ebp, dword ptr [0x4b43c0]
004056eb  mov        eax, dword ptr [ebp + 8]
004056ee  mov        ecx, 2
004056f3  or         dword ptr [eax + 4], ecx
004056f6  mov        eax, dword ptr [ebp + 0xc]
004056f9  or         dword ptr [eax + 4], ecx
004056fc  mov        eax, dword ptr [ebp + 0x3600]
00405702  or         dword ptr [eax + 4], ecx
00405705  mov        ecx, dword ptr [ebp + 0x10]
00405708  xor        eax, eax
0040570a  xor        edx, edx
0040570c  cmp        dx, word ptr [ecx]
0040570f  push       esi
00405710  push       edi
00405711  mov        dword ptr [esp + 0x14], eax
00405715  mov        dword ptr [esp + 0x18], eax
00405719  jge        0x4057d0

// block 0040571f
0040571f  mov        ecx, dword ptr [ebp + 0x14]
00405722  mov        edx, dword ptr [ecx + eax*4]
00405725  mov        byte ptr [edx + 3], 1
00405729  mov        ecx, dword ptr [ebp + 0x14]
0040572c  mov        edi, dword ptr [ecx + eax*4]
0040572f  add        edi, 0x1c
00405732  cmp        word ptr [edi], 0
00405736  jl         0x4057af

// block 00405738
00405738  mov        ebx, dword ptr [esp + 0x14]
0040573c  imul       ebx, ebx, 0x4b4
00405742  jmp        0x405750

// block 00405750
00405750  movsx      edx, word ptr [edi + 4]
00405754  mov        esi, dword ptr [ebp + 0x1c8]
0040575a  mov        eax, dword ptr [ebp + 0x1c4]
00405760  add        esi, ebx
00405762  mov        dword ptr [esp + 0x1c], edx
00405766  mov        dword ptr [esp + 0x20], eax
0040576a  call       0x402520

// block 0040576f
0040576f  mov        ecx, dword ptr [esp + 0x1c]
00405773  push       ecx
00405774  mov        ecx, dword ptr [esp + 0x24]
00405778  push       esi
00405779  mov        byte ptr [esi + 0x49d], 0x10
00405780  mov        byte ptr [esi + 0x49c], 0x10
00405787  call       0x454d10

// block 0040578c
0040578c  mov        eax, dword ptr [esp + 0x14]
00405790  movsx      edx, word ptr [edi + 2]
00405794  mov        word ptr [edi + 6], ax
00405798  inc        eax
00405799  add        edi, edx
0040579b  add        ebx, 0x4b4
004057a1  cmp        word ptr [edi], 0
004057a5  mov        dword ptr [esp + 0x14], eax
004057a9  jge        0x405750

// block 004057ab
004057ab  mov        eax, dword ptr [esp + 0x18]

// block 004057af
004057af  mov        ecx, dword ptr [ebp + 0x10]
004057b2  movsx      edx, word ptr [ecx]
004057b5  inc        eax
004057b6  cmp        eax, edx
004057b8  mov        dword ptr [esp + 0x18], eax
004057bc  jl         0x40571f

// block 004057c2
004057c2  mov        eax, dword ptr [ebp + 0x1c]
004057c5  mov        dword ptr [ebp + 0x4c], eax
004057c8  pop        edi
004057c9  pop        esi
004057ca  pop        ebp
004057cb  pop        ebx
004057cc  add        esp, 0x14
004057cf  ret        

// block 004057d0
004057d0  mov        ecx, dword ptr [ebp + 0x1c]
004057d3  pop        edi
004057d4  pop        esi
004057d5  mov        dword ptr [ebp + 0x4c], ecx
004057d8  pop        ebp
004057d9  pop        ebx
004057da  add        esp, 0x14
004057dd  ret        
