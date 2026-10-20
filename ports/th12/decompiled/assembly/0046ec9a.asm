
// block 0046ec9a
0046ec9a  mov        edi, edi
0046ec9c  push       ebp
0046ec9d  mov        ebp, esp
0046ec9f  mov        eax, dword ptr [ebp + 8]
0046eca2  push       esi
0046eca3  lea        esi, [eax*8 + 0x4ad2b8]
0046ecaa  cmp        dword ptr [esi], 0
0046ecad  jne        0x46ecc2

// block 0046ecaf
0046ecaf  push       eax
0046ecb0  call       0x46ebd7

// block 0046ecb5
0046ecb5  pop        ecx
0046ecb6  test       eax, eax
0046ecb8  jne        0x46ecc2

// block 0046ecba
0046ecba  push       0x11
0046ecbc  call       0x472c0d

// block 0046ecc1
0046ecc1  pop        ecx

// block 0046ecc2
0046ecc2  push       dword ptr [esi]
0046ecc4  call       dword ptr [0x498088]

// block 0046ecca
0046ecca  pop        esi
0046eccb  pop        ebp
0046eccc  ret        
