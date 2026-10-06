#include "LaserSnapshot.hpp"
#include "GameState.hpp"
#include "LaserWorld.hpp"

namespace th12 {
void snapshotLasers(std::ostream& out) {
  out << "{\"nextSerial\":" << laserManager.serial() << ",\"nodes\":[";
  bool comma = false;
  for (const auto* object : laserManager.ordered()) {
    const auto& s = object->state;
    const auto& p = s.parameters;
    if (comma)
      out << ',';
    comma = true;
    out << "{\"serial\":" << s.serialId << ",\"lookupId\":" << s.lookupId
        << ",\"kind\":" << int(s.kind) << ",\"phase\":" << int(s.phase) << ",\"position\":["
        << s.position.x << ',' << s.position.y << ',' << s.position.z << "],\"velocity\":["
        << s.velocity.x << ',' << s.velocity.y << ',' << s.velocity.z << "],\"geometry\":["
        << s.angle << ',' << s.length << ',' << s.width << ',' << s.speed << ',' << s.headOffset
        << "],\"age\":[" << s.age.previous << ',' << s.age.current << ',' << s.age.value
        << "],\"grazeAge\":[" << s.grazeAge.previous << ',' << s.grazeAge.current << ','
        << s.grazeAge.value << "],\"extensionAge\":[" << s.extensionAge.previous << ','
        << s.extensionAge.current << ',' << s.extensionAge.value << "],\"wait\":["
        << s.wait.previous << ',' << s.wait.current << ',' << s.wait.value
        << "],\"cancellable\":" << s.cancellable << ",\"retirePending\":" << int(s.retirePending)
        << ",\"protection\":" << s.protection << ",\"extensionCursor\":" << s.cursor
        << ",\"activeExtensions\":" << s.activeExtensions << ",\"parameters\":[" << p.initialLength
        << ',' << p.maxLength << ',' << p.travelDistance << ',' << p.width << ',' << p.initialOffset
        << ',' << p.angularVelocity << ',' << p.delay << ',' << p.grow << ',' << p.hold << ','
        << p.shrink << ',' << p.nodeCount << ',' << p.type << ',' << p.color << ',' << p.flags
        << "],\"curve\":[";
    bool inner = false;
    for (const auto& n : s.nodes) {
      if (inner)
        out << ',';
      inner = true;
      out << '[' << n.position.x << ',' << n.position.y << ',' << n.position.z << ',' << n.angle
          << ',' << n.speed << ']';
    }
    out << "]}";
  }
  out << "]}";
}
} // namespace th12
