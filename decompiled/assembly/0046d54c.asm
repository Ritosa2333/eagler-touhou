
// block 0046d54c
0046d54c  mov        edi, edi
0046d54e  push       ebp
0046d54f  mov        ebp, esp
0046d551  sub        esp, 0x10
0046d554  push       0
0046d556  lea        ecx, [ebp - 0x10]
0046d559  call       0x46d3b4

// block 0046d55e
0046d55e  lea        eax, [ebp - 0x10]
0046d561  push       eax
0046d562  push       dword ptr [ebp + 0x14]
0046d565  push       dword ptr [ebp + 0x10]
0046d568  push       dword ptr [ebp + 0xc]
0046d56b  push       dword ptr [ebp + 8]
0046d56e  call       0x47676f

// block 0046d573
0046d573  add        esp, 0x14
0046d576  cmp        byte ptr [ebp - 4], 0
0046d57a  je         0x46d583

// block 0046d57c
0046d57c  mov        ecx, dword ptr [ebp - 8]
0046d57f  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0046d583
0046d583  leave      
0046d584  ret        
