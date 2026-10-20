
// block 00490b6a
00490b6a  mov        edi, edi
00490b6c  push       ebp
00490b6d  mov        ebp, esp
00490b6f  sub        esp, 0x10
00490b72  push       dword ptr [ebp + 8]
00490b75  lea        ecx, [ebp - 0x10]
00490b78  call       0x46d3b4

// block 00490b7d
00490b7d  push       dword ptr [ebp + 0x24]
00490b80  mov        edx, dword ptr [ebp + 0x14]
00490b83  push       dword ptr [ebp + 0x20]
00490b86  lea        ecx, [ebp - 0x10]
00490b89  push       dword ptr [ebp + 0x1c]
00490b8c  push       dword ptr [ebp + 0x18]
00490b8f  push       dword ptr [ebp + 0x10]
00490b92  push       dword ptr [ebp + 0xc]
00490b95  call       0x4907fa

// block 00490b9a
00490b9a  add        esp, 0x18
00490b9d  cmp        byte ptr [ebp - 4], 0
00490ba1  je         0x490baa

// block 00490ba3
00490ba3  mov        ecx, dword ptr [ebp - 8]
00490ba6  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 00490baa
00490baa  leave      
00490bab  ret        
