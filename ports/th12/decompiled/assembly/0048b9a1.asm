
// block 0048b9a1
0048b9a1  mov        edi, edi
0048b9a3  push       ebp
0048b9a4  mov        ebp, esp
0048b9a6  sub        esp, 0x10
0048b9a9  push       dword ptr [ebp + 0xc]
0048b9ac  lea        ecx, [ebp - 0x10]
0048b9af  call       0x46d3b4

// block 0048b9b4
0048b9b4  mov        eax, dword ptr [ebp - 0x10]
0048b9b7  cmp        dword ptr [eax + 0xac], 1
0048b9be  jle        0x48b9d3

// block 0048b9c0
0048b9c0  lea        eax, [ebp - 0x10]
0048b9c3  push       eax
0048b9c4  push       2
0048b9c6  push       dword ptr [ebp + 8]
0048b9c9  call       0x4758eb

// block 0048b9ce
0048b9ce  add        esp, 0xc
0048b9d1  jmp        0x48b9e3

// block 0048b9d3
0048b9d3  mov        eax, dword ptr [eax + 0xc8]
0048b9d9  mov        ecx, dword ptr [ebp + 8]
0048b9dc  movzx      eax, word ptr [eax + ecx*2]
0048b9e0  and        eax, 2

// block 0048b9e3
0048b9e3  cmp        byte ptr [ebp - 4], 0
0048b9e7  je         0x48b9f0

// block 0048b9e9
0048b9e9  mov        ecx, dword ptr [ebp - 8]
0048b9ec  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0048b9f0
0048b9f0  leave      
0048b9f1  ret        
