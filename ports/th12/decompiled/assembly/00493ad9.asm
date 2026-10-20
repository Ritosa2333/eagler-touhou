
// block 00493ad9
00493ad9  mov        edi, edi
00493adb  push       ebp
00493adc  mov        ebp, esp
00493ade  cmp        dword ptr [ebp + 8], 0
00493ae2  jne        0x493aed

// block 00493ae4
00493ae4  and        dword ptr [0x4d52d0], 0
00493aeb  pop        ebp
00493aec  ret        

// block 00493aed
00493aed  push       dword ptr [ebp + 8]
00493af0  call       0x475163

// block 00493af5
00493af5  pop        ecx
00493af6  mov        dword ptr [0x4d52d8], eax
00493afb  mov        dword ptr [0x4d52d0], 1
00493b05  pop        ebp
00493b06  ret        
