#pragma once
#include "../archive/archive.hpp"
#include <span>
namespace th20::source::progress {
struct CompressionNode {std::uint32_t parent,left,right;};
// 539550 uses the same dictionary as 5391f0. Its separate 8193-node search
// tree is reset every invocation; the final dictionary remains observable.
class LzssEncoder {
public:
    explicit LzssEncoder(std::array<std::uint8_t,8192>& dictionary):dictionary_(dictionary) {}
    Bytes encode(std::span<const std::uint8_t>); //539550
    const auto& tree() const noexcept {return tree_;}
private:
    std::array<std::uint8_t,8192>& dictionary_;
    std::array<CompressionNode,8193> tree_;
    void replace(std::uint32_t old_node,std::uint32_t new_node); //539a10
    void promote(std::uint32_t old_node,std::uint32_t new_node); //539180
    void remove(std::uint32_t); //5394c0 and539960
    unsigned insert(std::uint32_t,std::uint32_t& match); //539060
};
void encrypt(Bytes&,CryptParameters); //4103c0
}
