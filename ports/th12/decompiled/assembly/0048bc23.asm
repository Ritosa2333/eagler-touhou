
// block 0048bc23
0048bc23  mov        edi, edi
0048bc25  push       ebp
0048bc26  mov        ebp, esp
0048bc28  sub        esp, 0x10
0048bc2b  push       dword ptr [ebp + 0xc]
0048bc2e  lea        ecx, [ebp - 0x10]
0048bc31  call       0x46d3b4

// block 0048bc36
0048bc36  mov        eax, dword ptr [ebp - 0x10]
0048bc39  cmp        dword ptr [eax + 0xac], 1
0048bc40  jle        0x48bc58

// block 0048bc42
0048bc42  lea        eax, [ebp - 0x10]
0048bc45  push       eax
0048bc46  push       0x107
0048bc4b  push       dword ptr [ebp + 8]
0048bc4e  call       0x4758eb

// block 0048bc53
0048bc53  add        esp, 0xc
0048bc56  jmp        0x48bc6a

// block 0048bc58
0048bc58  mov        eax, dword ptr [eax + 0xc8]
0048bc5e  mov        ecx, dword ptr [ebp + 8]
0048bc61  movzx      eax, word ptr [eax + ecx*2]
0048bc65  and        eax, 0x107

// block 0048bc6a
0048bc6a  cmp        byte ptr [ebp - 4], 0
0048bc6e  je         0x48bc77

// block 0048bc70
0048bc70  mov        ecx, dword ptr [ebp - 8]
0048bc73  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0048bc77
0048bc77  leave      
0048bc78  ret        
