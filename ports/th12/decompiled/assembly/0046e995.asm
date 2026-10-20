
// block 0046e995
0046e995  mov        edi, edi
0046e997  push       ebp
0046e998  mov        ebp, esp
0046e99a  push       esi
0046e99b  call       0x46e982

// block 0046e9a0
0046e9a0  mov        ecx, dword ptr [ebp + 8]
0046e9a3  push       ecx
0046e9a4  mov        dword ptr [eax], ecx
0046e9a6  call       0x46e92d

// block 0046e9ab
0046e9ab  pop        ecx
0046e9ac  mov        esi, eax
0046e9ae  call       0x46e96f

// block 0046e9b3
0046e9b3  mov        dword ptr [eax], esi
0046e9b5  pop        esi
0046e9b6  pop        ebp
0046e9b7  ret        
