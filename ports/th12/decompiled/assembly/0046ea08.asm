
// block 0046ea08
0046ea08  mov        edi, edi
0046ea0a  push       ebp
0046ea0b  mov        ebp, esp
0046ea0d  call       0x4753ee

// block 0046ea12
0046ea12  test       eax, eax
0046ea14  jne        0x46ea1b

// block 0046ea16
0046ea16  push       0xc
0046ea18  pop        eax
0046ea19  pop        ebp
0046ea1a  ret        

// block 0046ea1b
0046ea1b  call       0x46e982

// block 0046ea20
0046ea20  mov        ecx, dword ptr [ebp + 8]
0046ea23  mov        dword ptr [eax], ecx
0046ea25  xor        eax, eax
0046ea27  pop        ebp
0046ea28  ret        
