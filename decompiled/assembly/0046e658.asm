
// block 0046e658
0046e658  mov        eax, 0x5a4d
0046e65d  cmp        word ptr [0x400000], ax
0046e664  jne        0x46e69c

// block 0046e666
0046e666  mov        eax, dword ptr [0x40003c]
0046e66b  cmp        dword ptr [eax + 0x400000], 0x4550
0046e675  jne        0x46e69c

// block 0046e677
0046e677  mov        ecx, 0x10b
0046e67c  cmp        word ptr [eax + 0x400018], cx
0046e683  jne        0x46e69c

// block 0046e685
0046e685  cmp        dword ptr [eax + 0x400074], 0xe
0046e68c  jbe        0x46e69c

// block 0046e68e
0046e68e  xor        ecx, ecx
0046e690  cmp        dword ptr [eax + 0x4000e8], ecx
0046e696  setne      cl
0046e699  mov        eax, ecx
0046e69b  ret        

// block 0046e69c
0046e69c  xor        eax, eax
0046e69e  ret        
