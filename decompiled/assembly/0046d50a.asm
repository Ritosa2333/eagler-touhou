
// block 0046d50a
0046d50a  mov        edi, edi
0046d50c  push       ebp
0046d50d  mov        ebp, esp
0046d50f  sub        esp, 0x10
0046d512  push       0
0046d514  lea        ecx, [ebp - 0x10]
0046d517  call       0x46d3b4

// block 0046d51c
0046d51c  lea        eax, [ebp - 0x10]
0046d51f  push       eax
0046d520  push       dword ptr [ebp + 0x20]
0046d523  push       dword ptr [ebp + 0x1c]
0046d526  push       dword ptr [ebp + 0x18]
0046d529  push       dword ptr [ebp + 0x14]
0046d52c  push       dword ptr [ebp + 0x10]
0046d52f  push       dword ptr [ebp + 0xc]
0046d532  push       dword ptr [ebp + 8]
0046d535  call       0x476077

// block 0046d53a
0046d53a  add        esp, 0x20
0046d53d  cmp        byte ptr [ebp - 4], 0
0046d541  je         0x46d54a

// block 0046d543
0046d543  mov        ecx, dword ptr [ebp - 8]
0046d546  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0046d54a
0046d54a  leave      
0046d54b  ret        
