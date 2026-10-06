
// block 0046ed7a
0046ed7a  mov        edi, edi
0046ed7c  push       ebp
0046ed7d  mov        ebp, esp
0046ed7f  push       0x140
0046ed84  push       0
0046ed86  push       dword ptr [0x4b3c04]
0046ed8c  call       dword ptr [0x4981d4]

// block 0046ed92
0046ed92  mov        dword ptr [0x4d6444], eax
0046ed97  test       eax, eax
0046ed99  jne        0x46ed9d

// block 0046ed9b
0046ed9b  pop        ebp
0046ed9c  ret        

// block 0046ed9d
0046ed9d  mov        ecx, dword ptr [ebp + 8]
0046eda0  and        dword ptr [0x4b3d58], 0
0046eda7  and        dword ptr [0x4d6440], 0
0046edae  mov        dword ptr [0x4d644c], eax
0046edb3  xor        eax, eax
0046edb5  mov        dword ptr [0x4d6448], ecx
0046edbb  mov        dword ptr [0x4d6450], 0x10
0046edc5  inc        eax
0046edc6  pop        ebp
0046edc7  ret        
