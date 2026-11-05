# Evaluated immediately before linking, never at page open or deployment.
string(TIMESTAMP built_at "%Y-%m-%dT%H:%M:%SZ" UTC)
file(WRITE "${OUTPUT}" "Module['eaglerBuiltAt']='${built_at}';\n")
