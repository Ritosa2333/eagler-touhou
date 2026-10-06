
// block 0044d620
0044d620  push       0x1a
0044d622  call       0x44d640

// block 0044d627
0044d627  test       al, al
0044d629  je         0x44d62e

// block 0044d62b
0044d62b  mov        al, 1
0044d62d  ret        

// block 0044d62e
0044d62e  push       0x15
0044d630  call       0x44d640

// block 0044d635
0044d635  ret        
