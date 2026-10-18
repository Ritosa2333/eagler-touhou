# Shared client and service contracts

These ECMAScript modules are the executable authority shared by Launcher,
standalone services and other clients. They have no Launcher, DOM, build-cache,
or service-process dependency. TypeScript declarations preserve the existing
public types; Launcher source files only re-export these definitions.

The initial implementations were moved from Launcher through its existing
TypeScript compiler without changing the algorithms or protocol. Future edits
belong here. Keep the corresponding declarations and consumer type checks in
sync. The existing product catalog remains the single product-rule authority.

This directory inherits Launcher's GPL-3.0-or-later license, included in LICENSE.
The existing C++/browser common library retains its own license.
