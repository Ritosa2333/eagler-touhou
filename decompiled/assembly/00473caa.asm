
// block 00473caa
00473caa  mov        edi, edi
00473cac  push       ebp
00473cad  mov        ebp, esp
00473caf  sub        esp, 0x10
00473cb2  push       0
00473cb4  lea        ecx, [ebp - 0x10]
00473cb7  call       0x46d3b4

// block 00473cbc
00473cbc  mov        eax, dword ptr [ebp - 0xc]
00473cbf  cmp        dword ptr [eax + 8], 0
00473cc3  je         0x473cd7

// block 00473cc5
00473cc5  cmp        byte ptr [ebp - 4], 0
00473cc9  mov        eax, dword ptr [eax + 4]
00473ccc  je         0x473ce6

// block 00473cce
00473cce  mov        ecx, dword ptr [ebp - 8]
00473cd1  and        dword ptr [ecx + 0x70], 0xfffffffd
00473cd5  leave      
00473cd6  ret        

// block 00473cd7
00473cd7  cmp        byte ptr [ebp - 4], 0
00473cdb  je         0x473ce4

// block 00473cdd
00473cdd  mov        eax, dword ptr [ebp - 8]
00473ce0  and        dword ptr [eax + 0x70], 0xfffffffd

// block 00473ce4
00473ce4  xor        eax, eax

// block 00473ce6
00473ce6  leave      
00473ce7  ret        
