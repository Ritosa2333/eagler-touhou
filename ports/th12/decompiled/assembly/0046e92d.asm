
// block 0046e92d
0046e92d  mov        edi, edi
0046e92f  push       ebp
0046e930  mov        ebp, esp
0046e932  mov        eax, dword ptr [ebp + 8]
0046e935  xor        ecx, ecx

// block 0046e937
0046e937  cmp        eax, dword ptr [ecx*8 + 0x4ad140]
0046e93e  je         0x46e953

// block 0046e940
0046e940  inc        ecx
0046e941  cmp        ecx, 0x2d
0046e944  jb         0x46e937

// block 0046e946
0046e946  lea        ecx, [eax - 0x13]
0046e949  cmp        ecx, 0x11
0046e94c  ja         0x46e95c

// block 0046e94e
0046e94e  push       0xd
0046e950  pop        eax
0046e951  pop        ebp
0046e952  ret        

// block 0046e953
0046e953  mov        eax, dword ptr [ecx*8 + 0x4ad144]
0046e95a  pop        ebp
0046e95b  ret        

// block 0046e95c
0046e95c  add        eax, 0xffffff44
0046e961  push       0xe
0046e963  pop        ecx
0046e964  cmp        ecx, eax
0046e966  sbb        eax, eax
0046e968  and        eax, ecx
0046e96a  add        eax, 8
0046e96d  pop        ebp
0046e96e  ret        
