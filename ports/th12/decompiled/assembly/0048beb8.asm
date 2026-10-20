
// block 0048beb8
0048beb8  mov        edi, edi
0048beba  push       ebp
0048bebb  mov        ebp, esp
0048bebd  movzx      eax, byte ptr [ebp + 8]
0048bec1  push       eax
0048bec2  call       0x48bc79

// block 0048bec7
0048bec7  pop        ecx
0048bec8  test       eax, eax
0048beca  jne        0x48bed4

// block 0048becc
0048becc  cmp        byte ptr [ebp + 8], 0x5f
0048bed0  je         0x48bed4

// block 0048bed2
0048bed2  pop        ebp
0048bed3  ret        

// block 0048bed4
0048bed4  xor        eax, eax
0048bed6  inc        eax
0048bed7  pop        ebp
0048bed8  ret        
