
// block 0048ed72
0048ed72  mov        edi, edi
0048ed74  push       ebp
0048ed75  mov        ebp, esp
0048ed77  sub        esp, 0x10
0048ed7a  push       dword ptr [ebp + 8]
0048ed7d  lea        ecx, [ebp - 0x10]
0048ed80  call       0x46d3b4

// block 0048ed85
0048ed85  cmp        dword ptr [ebp + 0x14], -1
0048ed89  jge        0x48ed8f

// block 0048ed8b
0048ed8b  xor        eax, eax
0048ed8d  jmp        0x48eda1

// block 0048ed8f
0048ed8f  push       dword ptr [ebp + 0x18]
0048ed92  push       dword ptr [ebp + 0x14]
0048ed95  push       dword ptr [ebp + 0x10]
0048ed98  push       dword ptr [ebp + 0xc]
0048ed9b  call       dword ptr [0x498048]

// block 0048eda1
0048eda1  cmp        byte ptr [ebp - 4], 0
0048eda5  je         0x48edae

// block 0048eda7
0048eda7  mov        ecx, dword ptr [ebp - 8]
0048edaa  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0048edae
0048edae  leave      
0048edaf  ret        
