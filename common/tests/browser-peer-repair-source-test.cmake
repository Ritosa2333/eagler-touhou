file(READ "${SOURCE}" CONTENT)

foreach(REQUIRED IN ITEMS
    "EM_JS(int, eagler_peer_send_repair_to"
    "state.route !== 'rtc'"
    "const channel = state.peers.get(peerId)?.controlDc;"
    "channel.bufferedAmount > 32768"
    "channel.send(HEAPU8.slice(data, data + size));"
    "state.inputRepairSent = (state.inputRepairSent || 0) + 1;"
    "return eagler_peer_send_repair_to(peer, data, static_cast<int>(size)) != 0;"
)
    string(FIND "${CONTENT}" "${REQUIRED}" POSITION)
    if(POSITION EQUAL -1)
        message(FATAL_ERROR "missing bounded reliable repair contract: ${REQUIRED}")
    endif()
endforeach()

# Relay already provides a single reliable WebSocket path. Repair is only for
# the RTC fast/control split and must not create a second relay transmission.
string(FIND "${CONTENT}" "state.route === 'relay'" RELAY_POSITION)
string(FIND "${CONTENT}" "EM_JS(int, eagler_peer_send_repair_to" REPAIR_POSITION)
string(FIND "${CONTENT}" "EM_JS(int, eagler_peer_send_spectator" NEXT_POSITION)
math(EXPR REPAIR_LENGTH "${NEXT_POSITION} - ${REPAIR_POSITION}")
string(SUBSTRING "${CONTENT}" ${REPAIR_POSITION} ${REPAIR_LENGTH} REPAIR_BLOCK)
string(FIND "${REPAIR_BLOCK}" "state.route === 'relay'" REPAIR_RELAY_POSITION)
if(NOT REPAIR_RELAY_POSITION EQUAL -1)
    message(FATAL_ERROR "repair path must not duplicate relay traffic")
endif()
