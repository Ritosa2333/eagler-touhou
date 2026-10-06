
// block 0046cf81
0046cf81  push       0xc
0046cf83  push       0x4aaa78
0046cf88  call       0x46fcec

// block 0046cf8d
0046cf8d  and        dword ptr [ebp - 0x1c], 0
0046cf91  mov        esi, dword ptr [ebp + 8]
0046cf94  cmp        esi, dword ptr [0x4d6448]
0046cf9a  ja         0x46cfbe

// block 0046cf9c
0046cf9c  push       4
0046cf9e  call       0x46ec9a

// block 0046cfa3
0046cfa3  pop        ecx
0046cfa4  and        dword ptr [ebp - 4], 0
0046cfa8  push       esi
0046cfa9  call       0x46fa07

// block 0046cfae
0046cfae  pop        ecx
0046cfaf  mov        dword ptr [ebp - 0x1c], eax
0046cfb2  mov        dword ptr [ebp - 4], 0xfffffffe
0046cfb9  call       0x46cfc7

// block 0046cfbe
0046cfbe  mov        eax, dword ptr [ebp - 0x1c]
0046cfc1  call       0x46fd31

// block 0046cfc6
0046cfc6  ret        
