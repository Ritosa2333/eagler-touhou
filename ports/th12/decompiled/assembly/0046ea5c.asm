
// block 0046ea5c
0046ea5c  mov        edi, edi
0046ea5e  push       ebp
0046ea5f  mov        ebp, esp
0046ea61  xor        eax, eax
0046ea63  cmp        dword ptr [ebp + 8], eax
0046ea66  push       0
0046ea68  sete       al
0046ea6b  push       0x1000
0046ea70  push       eax
0046ea71  call       dword ptr [0x4981c0]

// block 0046ea77
0046ea77  mov        dword ptr [0x4b3c04], eax
0046ea7c  test       eax, eax
0046ea7e  jne        0x46ea82

// block 0046ea80
0046ea80  pop        ebp
0046ea81  ret        

// block 0046ea82
0046ea82  xor        eax, eax
0046ea84  inc        eax
0046ea85  mov        dword ptr [0x4d6458], eax
0046ea8a  pop        ebp
0046ea8b  ret        
