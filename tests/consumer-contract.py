#!/usr/bin/env python3
"""Guard the TH06/TH07 shared netplay authority convergence slices."""

from __future__ import annotations

from pathlib import Path
import os
import subprocess


COMMON = Path(__file__).resolve().parents[1]
WORKSPACE = COMMON.parent
COMMON_URL = "https://github.com/YomotsuHisami/eagler-common.git"


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def git(root: Path, *args: str) -> str:
    return subprocess.check_output(
        ["git", *args], cwd=root, text=True, encoding="utf-8"
    ).strip()


def consumer_root(name: str) -> Path:
    env_name = "EAGLER_" + name.split("-")[0].upper() + "_ROOT"
    override = os.environ.get(env_name)
    return Path(override).resolve() if override else WORKSPACE / name


def check_consumer(name: str, source_var: str, magic: str, transport_tag: str) -> None:
    root = consumer_root(name)
    cmake = read(root / "CMakeLists.txt")
    gitmodules = read(root / ".gitmodules")
    submodule_path = root / "third_party" / "eagler-common"

    require(
        '[submodule "third_party/eagler-common"]' in gitmodules
        and "path = third_party/eagler-common" in gitmodules
        and f"url = {COMMON_URL}" in gitmodules,
        f"{name}: canonical eagler-common submodule declaration is missing",
    )
    gitlink = git(root, "ls-files", "--stage", "third_party/eagler-common")
    fields = gitlink.split()
    require(
        len(fields) >= 2 and fields[0] == "160000",
        f"{name}: eagler-common is not recorded as a Git submodule gitlink",
    )
    require(submodule_path.is_dir(), f"{name}: eagler-common submodule is not checked out")
    checkout = git(submodule_path, "rev-parse", "HEAD")
    require(
        fields[1] == checkout,
        f"{name}: checked-out eagler-common revision does not match the pinned gitlink",
    )

    require(
        "include(\"${EAGLER_COMMON_ROOT}/cmake/EaglerCommon.cmake\")" in cmake,
        f"{name}: common CMake component is not loaded",
    )
    require(
        "eagler_common_link_netplay_base(${TH_EXEC_NAME})" in cmake,
        f"{name}: common netplay target is not linked",
    )
    require(
        "eagler_common_link_browser_peer_transport(${TH_EXEC_NAME})" in cmake,
        f"{name}: common browser peer transport target is not linked",
    )
    require(
        f"eagler_common_append_netplay_base_sources({source_var})" not in cmake
        and "eagler_common_add_include_path(${TH_EXEC_NAME})" not in cmake,
        f"{name}: legacy source/include wiring remains",
    )
    for stem in ("NetplayProtocol", "NetplayCore", "NetplaySession", "WebSocketTransport"):
        header = read(root / "src/netplay" / f"{stem}.hpp")
        source = read(root / "src/netplay" / f"{stem}.cpp")
        require(
            f"#include <eagler/netplay/{stem}.hpp>" in header,
            f"{name}: {stem} header is not a forwarding shim",
        )
        require(
            "implementation authority lives in eagler-common" in header,
            f"{name}: {stem} header does not document common authority",
        )
        require(
            "CMake compiles the eagler-common implementation source" in source,
            f"{name}: {stem} source is not a compatibility marker",
        )
        code_lines = [
            line
            for line in source.splitlines()
            if line.strip() and not line.lstrip().startswith("//")
        ]
        require(not code_lines, f"{name}: {stem} compatibility source contains implementation code")
    config = read(root / "src/netplay/NetplayProtocolConfig.hpp")
    require(
        f"MagicGame = '{magic}'" in config,
        f"{name}: protocol capability seam changed unexpectedly",
    )
    transport_config = read(root / "src/netplay/NetplayTransportConfig.hpp")
    require(
        f'Tag[] = "{transport_tag}"' in transport_config,
        f"{name}: browser transport tag seam changed unexpectedly",
    )
    transport_header = read(root / "src/netplay/BrowserPeerTransport.hpp")
    transport_source = read(root / "src/netplay/BrowserPeerTransport.cpp")
    require(
        "#include <eagler/netplay/BrowserPeerTransport.hpp>" in transport_header
        and "implementation authority lives in eagler-common" in transport_header,
        f"{name}: BrowserPeerTransport header is not a common forwarding shim",
    )
    require(
        "CMake compiles the eagler-common implementation source" in transport_source
        and not [
            line for line in transport_source.splitlines()
            if line.strip() and not line.lstrip().startswith("//")
        ],
        f"{name}: BrowserPeerTransport compatibility source contains implementation code",
    )
    require(
        "src/netplay/NetplayProtocol.cpp" not in cmake
        and "src/netplay/NetplayCore.cpp" not in cmake
        and "src/netplay/BrowserPeerTransport.cpp" not in cmake,
        f"{name}: common netplay authority still compiles from a title-local implementation",
    )


def main() -> None:
    for relative in (
        "CMakeLists.txt",
        "include/eagler/netplay/NetplayProtocol.hpp",
        "src/netplay/NetplayProtocol.cpp",
        "include/eagler/netplay/NetplayCore.hpp",
        "src/netplay/NetplayCore.cpp",
        "include/eagler/netplay/NetplaySession.hpp",
        "src/netplay/NetplaySession.cpp",
        "include/eagler/netplay/WebSocketTransport.hpp",
        "src/netplay/WebSocketTransport.cpp",
        "include/eagler/netplay/BrowserPeerTransport.hpp",
        "src/netplay/BrowserPeerTransport.cpp",
        "cmake/EaglerCommon.cmake",
    ):
        require((COMMON / relative).is_file(), f"missing common file: {relative}")

    common_cmake = read(COMMON / "CMakeLists.txt")
    require(
        "add_library(eagler::netplay_base ALIAS eagler_common_netplay_base)" in common_cmake,
        "common netplay component target is missing",
    )
    require(
        "add_library(eagler::browser_peer_transport ALIAS eagler_common_browser_peer_transport)" in common_cmake,
        "common browser peer transport component target is missing",
    )

    check_consumer("th06-eagler", "TH06_SOURCES", "6", "th06")
    check_consumer("th07-eagler", "SOURCES", "7", "th07")
    print("eagler-common consumer contract: PASS (TH06/TH07 shared netplay authority)")


if __name__ == "__main__":
    main()
