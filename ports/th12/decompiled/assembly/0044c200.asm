
// block 0044c200
0044c200  mov        ecx, dword ptr [0x4b4538]
0044c206  shl        ecx, 4
0044c209  add        ecx, 0x4cf2b0
0044c20f  call       0x44b610

// block 0044c214
0044c214  test       al, al
0044c216  je         0x44c224

// block 0044c218
0044c218  mov        eax, 1
0044c21d  add        dword ptr [0x4b4538], eax
0044c223  ret        

// block 0044c224
0044c224  xor        al, al
0044c226  ret        
