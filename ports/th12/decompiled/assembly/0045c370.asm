
// block 0045c370
0045c370  test       ecx, ecx
0045c372  jle        0x45c391

// block 0045c374
0045c374  add        eax, 0x10
0045c377  push       esi
0045c378  jmp        0x45c380

// block 0045c380
0045c380  mov        esi, dword ptr [edx + 0x3bc]
0045c386  mov        dword ptr [eax], esi
0045c388  add        eax, 0x1c
0045c38b  sub        ecx, 1
0045c38e  jne        0x45c380

// block 0045c390
0045c390  pop        esi

// block 0045c391
0045c391  xor        eax, eax
0045c393  ret        
