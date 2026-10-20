
// block 004d9aed
004d9aed  mov        esi, 0xbd24d1ad
004d9af2  pop        esp

// block 004d9af3
004d9af3  out        dx, eax

// block 004d9b56
004d9b56  xchg       esp, eax
004d9b57  in         al, 0xdb
004d9b59  mov        al, 0x26
004d9b5b  pop        ss
004d9b5c  pop        edi
004d9b5d  leave      
004d9b5e  jb         0x4d9af3

// block 004d9b60
004d9b60  sbb        bh, bl
004d9b62  jp         0x4d9aed

// block 004d9b64
004d9b64  fimul      word ptr [ebp + 0x2d2b4d32]
004d9b6a  daa        
004d9b6b  xor        edi, edx
004d9b6d  cmp        edx, dword ptr [edi]
004d9b6f  sbb        al, 0xe6
