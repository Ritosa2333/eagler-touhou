
// block 0043fc00
0043fc00  push       ebp
0043fc01  mov        ebp, esp
0043fc03  and        esp, 0xfffffff8
0043fc06  push       ecx
0043fc07  push       edi
0043fc08  mov        edi, eax
0043fc0a  mov        eax, dword ptr [edi + 0x1c]
0043fc0d  add        eax, -8
0043fc10  cmp        eax, 7
0043fc13  ja         0x43fc61

// block 0043fc15
0043fc15  jmp        dword ptr [eax*4 + 0x43fc6c]

// block 0043fc1c
0043fc1c  call       0x446c20

// block 0043fc21
0043fc21  mov        eax, 1
0043fc26  pop        edi
0043fc27  mov        esp, ebp
0043fc29  pop        ebp
0043fc2a  ret        

// block 0043fc2b
0043fc2b  push       edi
0043fc2c  call       0x447e10

// block 0043fc31
0043fc31  mov        eax, 1
0043fc36  pop        edi
0043fc37  mov        esp, ebp
0043fc39  pop        ebp
0043fc3a  ret        

// block 0043fc3b
0043fc3b  push       edi
0043fc3c  call       0x448690

// block 0043fc41
0043fc41  mov        eax, 1
0043fc46  pop        edi
0043fc47  mov        esp, ebp
0043fc49  pop        ebp
0043fc4a  ret        

// block 0043fc4b
0043fc4b  push       edi
0043fc4c  call       0x448fb0

// block 0043fc51
0043fc51  mov        eax, 1
0043fc56  pop        edi
0043fc57  mov        esp, ebp
0043fc59  pop        ebp
0043fc5a  ret        

// block 0043fc5b
0043fc5b  push       edi
0043fc5c  call       0x4463c0

// block 0043fc61
0043fc61  mov        eax, 1
0043fc66  pop        edi
0043fc67  mov        esp, ebp
0043fc69  pop        ebp
0043fc6a  ret        
