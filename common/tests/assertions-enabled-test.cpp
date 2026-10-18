#include <cassert>

int main()
{
    int evaluated = 0;
    assert(++evaluated == 1);
    // This must still fail if a build accidentally removes the assert above.
    return evaluated == 1 ? 0 : 1;
}
