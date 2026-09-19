---
name: mapdata-pugixml-migration
description: DONE 2026-09-19 — MapData's XML parsing migrated from vendored pugixml to boost::property_tree; pugixml deleted
metadata:
  type: project
---

Part of [[project-overview]] / [[component-game-dlls]]. Completed 2026-09-19 on the Linux (EC2) box.

**What changed:** `MerchantOfVenus/MapData.cpp`/`.hpp` now parse `MerchantOfVenusMap.xml` via `boost::property_tree::read_xml()` (with the `trim_whitespace` flag — needed, since it's off by default and indentation would otherwise show up as spurious `<xmltext>` pseudo-children). The vendored `pugixml.hpp`/`.cpp`/`pugiconfig.hpp` are deleted, along with their reference in `MerchantOfVenus/tca/Makefile`'s `OBJS`.

**Key property_tree gotchas hit during the rewrite** (useful if this pattern gets reused elsewhere in the codebase):
- Attributes live under a pseudo-child keyed `"<xmlattr>"` on each element's own `ptree`, not on some separate accessor — `node.get_child_optional("<xmlattr>")` and iterate that.
- Because attributes are modeled as a child, any element that both has attributes *and* real child elements (e.g. `<line name="...">`, `<solarsystem name="...">`, `<space ...>`) needs its child-iteration loop to explicitly skip the `"<xmlattr>"` key or it gets misidentified as an "unknown child".
- `ptree`/its exception types (`ptree_error`, `xml_parser_error`, etc.) all derive from `std::runtime_error`, so the existing `BOOST_CHECK_THROW(..., std::runtime_error)` characterization tests kept working with zero test changes.

**How this was validated:** [[merchant_of_venus_random_refactor|characterization tests]] written earlier in `MerchantOfVenus/tests/MapDataTest.cpp` (9 fixture-based validation-rule tests + a happy path + a smoke test against the real production map) were run against the old pugixml implementation as a baseline (all passing), then re-run unchanged against the new property_tree implementation — same 36/36 `MoVunittests.exe` cases pass, confirming behavioral parity.

**Toolchain note, now resolved:** the earlier blocker recorded here (MinGW `win32`-thread-model breaking Boost.Regex's internal `std::mutex` use) turned out to be Windows/MSYS-specific and irrelevant now that dev has moved to the Linux EC2 box per [[build_environment]] — `OSTYPE=linux make MoVunittests.exe` builds and links clean there with system boost via `-lboost_regex` etc.

**Two unrelated pre-existing build gaps, fixed in passing 2026-09-19 (both predate and are unrelated to the pugixml migration itself):**

1. **Missing `-fPIC`:** `MerchantOfVenus/tca/Makefile`'s default target links `MerchantOfVenus.${SHLIBEXT}` as a Linux shared object, but nothing in the Makefile passed `-fPIC`, so plain `make` failed at the link step (`relocation R_X86_64_32 against .bss`) — reproduced on every `.o` in the DLL, not just MapData.o. Fixed by adding `PICFLAG=-fPIC` to the `ifeq (${OSTYPE},linux)` block and threading `${PICFLAG}` into the `GCC` compile-flags variable (left unset on the `msys` branch, since MinGW's PE-based DLLs don't need/reliably accept `-fPIC`). `gamecommon/Makefile` and `common/Makefile` already built with `-fPIC` unconditionally, so only `tca/Makefile` needed the change.
2. **Wrong shared-object extension on Linux:** the Makefile unconditionally named the built artifact `MerchantOfVenus.dll`, but `common/SystemSpecificDynamicLoading.cpp`'s `GenericLibraryLoader` (used by `server/GameBox.cpp` to load game modules) does `dlopen(i_prefix + ".so")` on the `linux`/`FreeBSD` branch and only uses `LoadLibrary(i_prefix + ".dll")` under `msys` — so a Linux server would never have found the built artifact at all, extension mismatch. Fixed by adding an `SHLIBEXT` var (`dll` under msys, `so` under linux) and using `MerchantOfVenus.${SHLIBEXT}` for the build target, `serverinstall` recipe, and (both extensions, defensively) `clean`. **Note: the identical `.dll`-hardcoded pattern exists in `Outpost/tca/Makefile` and `AOR/Makefile` too — not fixed, since this session's work was scoped to MerchantOfVenus, but worth the same fix whenever those games' Linux builds are next touched.**

Verified for MoV: `OSTYPE=linux make clean && OSTYPE=linux make` builds `MerchantOfVenus.so`; `python3 -c "import ctypes; ctypes.CDLL('./MerchantOfVenus.so')"` loads it and resolves the `Initialize` C-ABI symbol; `MoVunittests.exe` still passes all 36 cases.
