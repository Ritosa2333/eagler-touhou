"""Aggregate CDP samples by function, counting a recursive name once per sample.

Usage: python summarize-profile.py input.cpuprofile output.json
Times are sampling weights in the throttled renderer, including startup.
"""
import collections
import hashlib
import json
import pathlib
import sys

source = pathlib.Path(sys.argv[1])
profile = json.loads(source.read_text(encoding="utf-8"))
nodes = {node["id"]: node for node in profile["nodes"]}
parents = {child: node["id"] for node in profile["nodes"] for child in node.get("children", [])}
own, inclusive, checkpoints = collections.Counter(), collections.Counter(), collections.Counter()
targets = {
    "gameLoop": "th17_loop_start",
    "capture": "RoomRuntime::capture(",
    "animationCheckpoint": "AnmCheckpoint::AnmCheckpoint(",
    "draw": "SceneDisplay::draw(",
    "drawSubmission": "Renderer::issue(",
    "audioPump": "AudioDevice::pump(",
    "animationOpen": "AnmResource::open(",
    "stageLoad": "StageAssets::load(",
}
totals = collections.Counter()
loop_totals = collections.Counter()
for node_id, weight in zip(profile["samples"], profile["timeDeltas"]):
    leaf = nodes[node_id]["callFrame"]["functionName"]
    own[leaf] += weight
    names = set()
    while node_id is not None:
        names.add(nodes[node_id]["callFrame"]["functionName"])
        node_id = parents.get(node_id)
    for name in names:
        inclusive[name] += weight
    for key, pattern in targets.items():
        if any(pattern in name for name in names):
            totals[key] += weight
            if any(targets["gameLoop"] in name for name in names):
                loop_totals[key] += weight
    if any(targets["animationCheckpoint"] in name for name in names):
        checkpoints[leaf] += weight

rows = lambda counter: [{"name": name, "weightedMs": round(weight / 1000, 3)}
                        for name, weight in counter.most_common(20)]
result = {
    "sha256": hashlib.sha256(source.read_bytes()).hexdigest(),
    "durationSeconds": (profile["endTime"] - profile["startTime"]) / 1e6,
    "samples": len(profile["samples"]),
    "samplingIntervalUs": 1000,
    "inclusiveWeightedMs": {key: round(totals[key] / 1000, 3) for key in targets},
    "insideGameLoopWeightedMs": {key: round(loop_totals[key] / 1000, 3) for key in targets},
    "selfTop": rows(own),
    "inclusiveTop": rows(inclusive),
    "animationCheckpointSelfTop": rows(checkpoints),
}
pathlib.Path(sys.argv[2]).write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
print(json.dumps(result["inclusiveWeightedMs"]))
