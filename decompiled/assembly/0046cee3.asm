
// block 0046cee3
0046cee3  mov        eax, dword ptr [ebp - 0x14]
0046cee6  mov        dword ptr [ebp - 0x1c], eax
0046cee9  mov        eax, dword ptr [ebp - 0x1c]
0046ceec  mov        eax, dword ptr [eax]
0046ceee  mov        dword ptr [ebp - 0x20], eax
0046cef1  mov        eax, dword ptr [ebp - 0x20]
0046cef4  cmp        dword ptr [eax], 0xe06d7363
0046cefa  je         0x46cf07

// block 0046cefc
0046cefc  mov        dword ptr [ebp - 0x24], 0
0046cf03  mov        eax, dword ptr [ebp - 0x24]
0046cf06  ret        

// block 0046cf07
0046cf07  call       0x472b48
