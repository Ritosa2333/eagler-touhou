
// block 0046d260
0046d260  push       ebp
0046d261  mov        ebp, esp
0046d263  push       edi
0046d264  mov        edi, dword ptr [ebp + 8]
0046d267  xor        eax, eax
0046d269  or         ecx, 0xffffffff

// block 0046d26c
0046d26c  repne scasb al, byte ptr es:[edi]

// block 0046d26e
0046d26e  add        ecx, 1
0046d271  neg        ecx
0046d273  sub        edi, 1
0046d276  mov        al, byte ptr [ebp + 0xc]
0046d279  std        

// block 0046d27a
0046d27a  repne scasb al, byte ptr es:[edi]

// block 0046d27c
0046d27c  add        edi, 1
0046d27f  cmp        byte ptr [edi], al
0046d281  je         0x46d287

// block 0046d283
0046d283  xor        eax, eax
0046d285  jmp        0x46d289

// block 0046d287
0046d287  mov        eax, edi

// block 0046d289
0046d289  cld        
0046d28a  pop        edi
0046d28b  leave      
0046d28c  ret        
