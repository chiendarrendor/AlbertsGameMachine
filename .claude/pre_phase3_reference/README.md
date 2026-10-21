# Pre-Phase-3 server XML (frozen reference copies)

`OutpostServer.xml` and `MerchantOfVenusServer.xml` in this directory are snapshots of those
files as of commit `e556e48^` (immediately *before* "full phase 3 server-side" rewrote them to
replace per-transition Broadcast/Unicast `<event>` sends with full-state-send JSON bodies).

Purpose: cross-reference. When converting an old client widget that consumed some specific
`<event name="...">`'s vars (e.g. `MEGAREQUEST`'s `i_resourcetype`/`i_maxmegas`), grep these
frozen copies for the event name to find exactly what C++ expression populated each var and
when it was sent (which transition/state, Unicast vs Broadcast, to whom) — then grep the live
`<Game>Server.xml` for where that same data landed in the FSS body (field name and renaming are
not guaranteed to match; e.g. `MEGAREQUEST`'s `i_resourcetype` became `megaRequest.resourceType`).

These are deliberately plain frozen copies, not symlinks or `git show` one-liners run fresh each
time — the point is a fast, stable `grep` target without needing to remember the commit hash.
They will never need updating; the pre-phase-3 protocol is gone for good once this conversion
work is done.
