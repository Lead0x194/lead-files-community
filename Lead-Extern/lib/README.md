# Lead-Extern/lib — x64 server + client dependency libraries

These are the **64-bit** builds of the third-party libraries the x64 server **and
client** link against. The old 32-bit `win32`/`win64` split was flattened during
the x64 port: `lib/` now holds the x64 libs directly (the 32-bit versions were
removed), and the x64 projects reference this directory in
`AdditionalLibraryDirectories`.

Most binaries **are committed** so the repo builds out of the box (incl.
`cryptlib-Release.lib`, ~42 MB). The sole exception is **`cryptlib-Debug.lib`**
(~158 MB): it exceeds GitHub's 100 MB blob limit and LFS uploads are blocked on
public forks, so it can't be vendored here — rebuild it from the steps below (or
attach it to a GitHub Release). All built with VS 2026 (v145 toolset), x64; Debug
libs use the `/MTd` runtime (to match the server's MultiThreadedDebug), Release
libs `/MT` — except DevIL which is a DLL (CRT-isolated).

| File | Source | How it was built |
|---|---|---|
| `cryptlib-Debug.lib` | in-repo `Lead-Extern/sources/cryptopp` | `msbuild cryptlib.vcxproj /p:Configuration=Debug /p:Platform=x64` (after the `integer.cpp`/`zdeflate.cpp` patch that drops `stdext::make_*checked_array_iterator`, removed from modern MSVC STL). Output copied to `cryptlib-Debug.lib`. |
| `lzo-2.10MT_d.lib` | lzo 2.10 (oberhumer.com) | `cl /c /MTd /O2 /I include src\*.c` then `lib /out:lzo-2.10MT_d.lib *.obj` in an x64 dev shell. |
| `mysqlclient.lib` | MariaDB Connector/C **3.3.8** (matches the bundled `include/mysql` headers) | `cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug -DWITH_SSL=SCHANNEL -DWITH_UNIT_TESTS=OFF -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_POLICY_VERSION_MINIMUM=3.5` then `cmake --build . --target mariadbclient`; `mariadbclient.lib` copied to `mysqlclient.lib`. Schannel backend ⇒ the server links `crypt32.lib`/`bcrypt.lib`. |
| `DevIL.lib` + `DevIL.dll` | DevIL master (github DentonW/DevIL) | `cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON -DIL_TESTS=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug -DJPEG_INCLUDE_DIR=<extern>/include/libjpeg -DJPEG_LIBRARY=<extern>/lib/libjpeg-9fMT_d.lib` then `cmake --build . --target IL` (comment out `add_subdirectory(src-ILUT)` in `DevIL/CMakeLists.txt` — pulls in GLUT, unused). **JPEG backend is wired in** so guild-mark upload accepts `.jpg`: the client decodes the picked image via `ilLoad` and ships compressed BGRA pixel blocks; the server only ever `ilSave`/`ilLoad`s `IL_TGA` mark blocks, so its DevIL needs no external codecs. Without JPEG, `ilLoad` of a `.jpg` returns `IL_INVALID_ENUM` (0x501) ⇒ in-game "The game does not support this picture". `DevIL.dll` must sit next to the exes at runtime. |

## x64 client-only dependency libs

The x64 client (`metin2client_debug.exe`) links these in addition to the shared
ones above. Built/fetched VS 2026 (v145), x64, `/MTd` to match the client.

| File | Source | How obtained |
|---|---|---|
| `d3dx9.lib` (+ `d3dx9d.lib`) | `Microsoft.DXSDK.D3DX` NuGet (Walbourn's repackage of the DXSDK June 2010 D3DX) | `nuget`/REST fetch the nupkg, copy `build/native/release/lib/x64/d3dx9.lib`. The legacy DXSDK installer itself fails headlessly (S1023 redist conflict) — the NuGet avoids it. Runtime: `d3dx9_43.dll` next to the exe. |
| `d3d9.lib` | Windows 10 SDK | copy `…\Windows Kits\10\Lib\<ver>\um\x64\d3d9.lib` here so it wins over the Win32 `lib/d3d9.lib` on the search path (needed for `Direct3DCreate9Ex`). |
| `python314.lib` (+ headers) | Python 3.14.3 x64 (python.org) | install python-3.14.3-amd64; copy `libs/python314.lib` and `include/` (flat headers + `cpython/`, `internal/` not needed) into `Lead-Extern/include/python`. Auto-linked by the `pyconfig.h` pragma; ScriptLib's `StdAfx.h` `#undef _DEBUG` trick makes Debug builds link the release lib. Runtime: `python314.dll`. |
| `mss64.lib` | Miles Sound System 9.3 SDK — [download](https://metin2.download/file/9jmaB37Fci6nXF6qbJTY8Q3RwIT3dmAF/) | extract `lib/mss64.lib`. Header `mss.h`+`rrcore.h` replace the Miles-6 header in `Lead-Extern/include/miles`. MilesLib was ported to the 9.3 API. Runtime: `mss64.dll`. |
| `granny2_x64.lib` | Granny 2.11.8.0 SDK — [download](https://metin2.download/file/uRynyND42I1Cw0hP4b2t5p9IJRvX3mVj/) | copy from `lib/win64`. Client pragma is `_WIN64`-conditional (`granny2_x64.lib`). Runtime: `granny2_x64.dll`. |
| `WebView2Loader.lib` | `Microsoft.Web.WebView2` NuGet | copy `build/native/x64/WebView2Loader.dll.lib` → `WebView2Loader.lib`. Runtime: `WebView2Loader.dll`. |
| `SpeedTreeRT.lib` | SpeedTreeRT 1.6 SDK — [download](https://metin2.download/get/m13gRDN10q1bi1QvMk2IiMHJG2TiUEJh/) | added Debug\|x64 to `SpeedTreeRT.vcxproj` (v145, `/Zc:strictStrings-`, `_HAS_STD_BYTE=0`); removed the unused `LoadTree(KStream*)` overload (client-internal dep not in this drop) and matched `SetNumWindMatrices(unsigned int)` to the vendored public header. |
| `libjpeg-9fMT_d.lib` | IJG libjpeg 9f (`ijg.org/files/jpegsr9f.zip`) | `jconfig.vc`→`jconfig.h`, then `cl /c /MTd` the `j*.c` core (minus `jpegtran.c` + the alternate `jmem*` managers, keep `jmemnobs`) and `lib` into this exact name (the `jpegLibLink.h` pragma builds `libjpeg-9f` + runtime-model + `_d`). |

Runtime DLLs are staged next to the exe in `Lead-Client/`: granny2_x64, mss64,
python314, WebView2Loader, D3DX9_43, DevIL.

## Release (`/MT`) client libs

The **Release\|x64** client (`metin2client.exe`) is built with the non-debug static
CRT (`/MT`, `NDEBUG`, `_ITERATOR_DEBUG_LEVEL=0`). A `/MT` object cannot link against a
`/MTd` static lib (`LNK2038`), so the few **statically-linked** third-party libs need a
`/MT` Release counterpart alongside the `/MTd` Debug one. The import-lib deps (granny2,
mss64, python314, DevIL, WebView2Loader, d3d9/d3dx9) are CRT-isolated and unchanged. All
four below are x64, `/MT`, `NDEBUG`; verified with `dumpbin -directives` ⇒ `LIBCMT`
(never `LIBCMTD`/`MSVCRT`). They live in this same `lib/` dir with distinct names so the
config-specific `#pragma comment(lib,…)` / auto-link headers pick the right one per
configuration (Debug keeps using the existing `/MTd` libs unchanged).

| File | Source | How it was built |
|---|---|---|
| `cryptlib-Release.lib` | Crypto++ **8.9.0** (`git clone -b CRYPTOPP_8_9_0 weidai11/cryptopp`, matches the vendored 8.9.0 headers) | `msbuild cryptlib.vcxproj /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v145 /p:WholeProgramOptimization=false` — Crypto++'s Release config is already `/MT`+`NDEBUG`. Same modern-MSVC `.cpp` patch as the Debug lib: cap the `integer.cpp` (`>=1500`) and `zdeflate.cpp` (`>=1600`) version guards with `&& CRYPTOPP_MSC_VERSION < 1930` so the non-`stdext` fallback branches are taken (`stdext::*checked_array_iterator` were removed from the 14.51 STL). `.cpp`-only ⇒ ABI-compatible with the vendored headers. Output `x64/Output/Release/cryptlib.lib` → `cryptlib-Release.lib`. **Committed** (~42 MB; under GitHub's 100 MB limit). |
| `SpeedTreeRT-Release.lib` | SpeedTreeRT 1.6 SDK (same drop as the Debug lib) | `msbuild SpeedTreeRT.vcxproj /p:Configuration=Release /p:Platform=x64 /p:WholeProgramOptimization=false` (the project's Release\|x64 config is `/MT`+`NDEBUG`; `/GL` off so the lib carries readable, toolset-portable directives). `d3d9.h` resolves from the Windows 10 SDK; the dead Oct-2004 DXSDK include path is harmless. The client pragma was made config-specific — `_DEBUG` → `SpeedTreeRT.lib` (`/MTd`), else `SpeedTreeRT-Release.lib` (`/MT`) — since the SDK lib is referenced by a single hard-coded name. |
| `lzo-2.10MT.lib` | lzo 2.10 (oberhumer.com) | `cl /c /MT /O2 /DNDEBUG /I include src\*.c` then `lib /out:lzo-2.10MT.lib *.obj` in an x64 dev shell (Release twin of `lzo-2.10MT_d.lib`; `lzoLibLink.h` auto-links `lzo-2.10`+`MT`+(`_d` iff `_DEBUG`)). |
| `libjpeg-9fMT.lib` | IJG libjpeg 9f (`ijg.org/files/jpegsr9f.zip`) | `jconfig.vc`→`jconfig.h`, then `cl /c /MT /O2 /DNDEBUG` the `j*.c` core (the libjpeg object set, minus the cjpeg/djpeg/jpegtran app `main()`s; keep `jmemnobs`) and `lib` into this exact name (`jpegLibLink.h` builds `libjpeg-9f`+runtime-model+(`_d` iff `_DEBUG`)). Release twin of `libjpeg-9fMT_d.lib`. |
