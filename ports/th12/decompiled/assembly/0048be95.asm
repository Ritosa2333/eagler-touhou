
// block 0048be95
0048be95  mov        edi, edi
0048be97  push       ebp
0048be98  mov        ebp, esp
0048be9a  push       dword ptr [ebp + 0xc]
0048be9d  push       dword ptr [ebp + 8]
0048bea0  call       0x48bc23

// block 0048bea5
0048bea5  pop        ecx
0048bea6  pop        ecx
0048bea7  test       eax, eax
0048bea9  jne        0x48beb3

// block 0048beab
0048beab  cmp        dword ptr [ebp + 8], 0x5f
0048beaf  je         0x48beb3

// block 0048beb1
0048beb1  pop        ebp
0048beb2  ret        

// block 0048beb3
0048beb3  xor        eax, eax
0048beb5  inc        eax
0048beb6  pop        ebp
0048beb7  ret        
