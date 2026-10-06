
// block 0046e44a
0046e44a  mov        edi, edi
0046e44c  push       ebp
0046e44d  mov        ebp, esp
0046e44f  cmp        dword ptr [0x49d57c], 0
0046e456  je         0x46e46d

// block 0046e458
0046e458  push       0x49d57c
0046e45d  call       0x477a40

// block 0046e462
0046e462  pop        ecx
0046e463  test       eax, eax
0046e465  je         0x46e46d

// block 0046e467
0046e467  call       dword ptr [0x49d57c]

// block 0046e46d
0046e46d  call       0x4753ee

// block 0046e472
0046e472  test       eax, eax
0046e474  je         0x46e47d

// block 0046e476
0046e476  push       eax
0046e477  call       0x4755b0

// block 0046e47c
0046e47c  pop        ecx

// block 0046e47d
0046e47d  push       dword ptr [ebp + 8]
0046e480  call       dword ptr [0x4981e0]

// block 0046e486
0046e486  int3       
