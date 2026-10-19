#pragma once
#include "records.hpp"
#include "compression.hpp"
#include <functional>
namespace th20::source::progress {
using Decode=std::function<Bytes(const Bytes&,std::uint32_t)>;
using Encode=std::function<Bytes(const Bytes&)>;
int parse_snapshot(Snapshot&,const Decode&); //50eb70, real caller supplies process dictionary
Bytes serialize_snapshot(Snapshot&,const Encode&); //50f6b0 before OS file writes
void release_snapshot_buffers(Snapshot&) noexcept;
}
