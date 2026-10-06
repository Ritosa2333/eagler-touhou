
// block 0048bca9
0048bca9  mov        edi, edi
0048bcab  push       ebp
0048bcac  mov        ebp, esp
0048bcae  sub        esp, 0x10
0048bcb1  push       dword ptr [ebp + 0xc]
0048bcb4  lea        ecx, [ebp - 0x10]
0048bcb7  call       0x46d3b4

// block 0048bcbc
0048bcbc  mov        eax, dword ptr [ebp - 0x10]
0048bcbf  cmp        dword ptr [eax + 0xac], 1
0048bcc6  jle        0x48bcde

// block 0048bcc8
0048bcc8  lea        eax, [ebp - 0x10]
0048bccb  push       eax
0048bccc  push       0x157
0048bcd1  push       dword ptr [ebp + 8]
0048bcd4  call       0x4758eb

// block 0048bcd9
0048bcd9  add        esp, 0xc
0048bcdc  jmp        0x48bcf0

// block 0048bcde
0048bcde  mov        eax, dword ptr [eax + 0xc8]
0048bce4  mov        ecx, dword ptr [ebp + 8]
0048bce7  movzx      eax, word ptr [eax + ecx*2]
0048bceb  and        eax, 0x157

// block 0048bcf0
0048bcf0  cmp        byte ptr [ebp - 4], 0
0048bcf4  je         0x48bcfd

// block 0048bcf6
0048bcf6  mov        ecx, dword ptr [ebp - 8]
0048bcf9  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0048bcfd
0048bcfd  leave      
0048bcfe  ret        
