
// block 00460460
00460460  xor        ecx, ecx
00460462  add        edx, 0x4b50c0

// block 00460468
00460468  mov        eax, dword ptr [edx]
0046046a  test       eax, eax
0046046c  je         0x460480

// block 0046046e
0046046e  cmp        dword ptr [eax + 0x128], 0
00460475  jne        0x46048f

// block 00460477
00460477  cmp        dword ptr [eax + 0x124], 0
0046047e  jne        0x46048f

// block 00460480
00460480  inc        ecx
00460481  add        edx, 4
00460484  cmp        ecx, 0x20
00460487  jb         0x460468

// block 00460489
00460489  mov        eax, 1
0046048e  ret        

// block 0046048f
0046048f  xor        eax, eax
00460491  ret        
