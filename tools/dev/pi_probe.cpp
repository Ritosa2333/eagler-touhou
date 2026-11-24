#include "th03/assets/decode.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
int main(int argc, char **argv) {
    if (argc != 2) return 2;
    int passed = 0, failed = 0;
    for (const auto &entry : std::filesystem::directory_iterator(argv[1])) {
        if (entry.path().extension() != ".PI") continue;
        std::ifstream f(entry.path(), std::ios::binary | std::ios::ate);
        auto size = f.tellg();
        if (size < 0) return 3;
        std::vector<std::byte> bytes(static_cast<std::size_t>(size));
        f.seekg(0); f.read(reinterpret_cast<char *>(bytes.data()), size);
        auto result = th03::assets::decode_pi(bytes);
        if (!result) { ++failed; std::cout << "FAIL " << entry.path().filename().string() << '\n'; continue; }
        ++passed;
        std::cout << "PASS " << entry.path().filename().string() << ' ' << result->image.width << 'x' << result->image.height << '\n';
    }
    std::cout << "TOTAL " << passed << " passed " << failed << " failed\n";
    return failed ? 1 : 0;
}
