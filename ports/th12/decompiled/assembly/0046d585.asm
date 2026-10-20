
// block 0046d585
0046d585  mov        edi, edi
0046d587  push       ebp
0046d588  mov        ebp, esp
0046d58a  push       0
0046d58c  push       dword ptr [ebp + 8]
0046d58f  call       dword ptr [0x4981d8]

// block 0046d595
0046d595  test       eax, eax
0046d597  jne        0x46d5a1

// block 0046d599
0046d599  call       dword ptr [0x4980e4]

// block 0046d59f
0046d59f  jmp        0x46d5a3

// block 0046d5a1
0046d5a1  xor        eax, eax

// block 0046d5a3
0046d5a3  test       eax, eax
0046d5a5  je         0x46d5b3

// block 0046d5a7
0046d5a7  push       eax
0046d5a8  call       0x46e995

// block 0046d5ad
0046d5ad  pop        ecx
0046d5ae  or         eax, 0xffffffff
0046d5b1  pop        ebp
0046d5b2  ret        

// block 0046d5b3
0046d5b3  xor        eax, eax
0046d5b5  pop        ebp
0046d5b6  ret        
