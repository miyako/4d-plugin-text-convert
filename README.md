![version](https://img.shields.io/badge/version-18%2B-EB8E5F)
![platform](https://img.shields.io/static/v1?label=platform&message=mac-intel%20|%20mac-arm%20|%20win-64&color=blue)
[![license](https://img.shields.io/github/license/miyako/4d-plugin-text-convert)](LICENSE)
![downloads](https://img.shields.io/github/downloads/miyako/4d-plugin-text-convert/total)

# 4d-plugin-text-convert

Text Convert exposes 4D commands that drive ICU, libiconv, and each platform's native text-encoding APIs (Windows `IMultiLanguage2`/MLang; macOS `CFString` and the legacy Carbon Text Encoding Conversion Manager) to convert between 4D Text and byte-level Blob data in encodings that 4D's own `CONVERT FROM TEXT`/`Convert to text` don't support (e.g. EUC-JP, ISO-2022-JP-1), guess the likely encoding of an arbitrary Blob, and perform ICU-based Unicode transliteration and normalization. Results come back as 4D Text, Blob, Longint, or arrays of these, depending on the command.

Every conversion/detection command reports failure through a 4D Longint error code — except `JIS Convert to text` and `JIS Convert from text`, which return their result directly and signal failure with an **empty** Text/Blob instead. See [Requirements & platform notes](#requirements--platform-notes) before assuming a non-empty/zero result.

## Summary

| Command | Returns | Purpose |
|---|---|---|
| [ICU GET ENCODING LIST](#icu-get-encoding-list) | — (fills array) | List every ICU converter name available on this machine |
| [ICU Get good encodings](#icu-get-good-encodings) | Longint | Guess every encoding a Blob could plausibly be, via ICU's charset detector |
| [ICU Get best encoding](#icu-get-best-encoding) | Longint | Guess the single most likely encoding of a Blob, via ICU |
| [ICU Normalize text](#icu-normalize-text) | Longint | Apply Unicode Normalization (NFC/NFD/NFKC/NFKD/NFKC_CF) |
| [ICU Transform text](#icu-transform-text) | Longint | Apply an ICU transliteration ID or custom rule (script conversion, romanization, filtering, ...) |
| [ICU Get error text](#icu-get-error-text) | Text | Convert a raw ICU error code into a human-readable name |
| [ICU Convert to text](#icu-convert-to-text) | Longint | Decode a Blob in a named ICU encoding into 4D Text |
| [ICU Convert from text](#icu-convert-from-text) | Longint | Encode 4D Text into a Blob in a named ICU encoding |
| [ICONV Convert](#iconv-convert) | Longint | Convert a Blob between two libiconv encoding names |
| [JIS Convert to text](#jis-convert-to-text) | Text | Decode a JIS-variant-encoded Blob into 4D Text |
| [JIS Convert from text](#jis-convert-from-text) | Blob | Encode 4D Text into a JIS-variant Blob |
| [ICONV GET ENCODING LIST](#iconv-get-encoding-list) | — (fills array) | List every libiconv encoding name available on this machine |
| [CP GET ENCODING LIST](#cp-get-encoding-list) | — (fills arrays) | List every platform-native encoding, with its Windows codepage number |
| [CP Get best encoding](#cp-get-best-encoding) | Longint | Guess the single most likely encoding of a Blob, via the platform's native sniffer |
| [CP Convert from text](#cp-convert-from-text) | Longint | Encode 4D Text into a Blob using a Windows codepage number |
| [CP Convert to text](#cp-convert-to-text) | Longint | Decode a Blob into 4D Text using a Windows codepage number |
| [CP Get good encodings](#cp-get-good-encodings) | Longint | Guess every encoding a Blob could plausibly be, via the platform's native sniffer |

**Platforms:** macOS (Intel & Apple Silicon) and Windows, both 64-bit only (per the project's own badges above). 4D itself has no Linux runtime, so there's no Linux build to consider.

---

## Requirements & platform notes

- Requires 4D version 18 or later (per the plugin's own version badge).
- **Every parameter on every command is mandatory.** The plugin always reads every declared parameter unconditionally — there's no optional/omittable parameter on any command here.
- **Return-code convention:** most commands return a 4D Longint. `0` means success. `-1`, `-2`, and `-3` are this plugin's own codes (`ERR_INVALID_ENCODING`, `ERR_CONVERSION_FAILED`, `ERR_DETECTION_FAILED` respectively); any other nonzero value from an **ICU-family** command is a raw ICU `UErrorCode` — pass it to [`ICU Get error text`](#icu-get-error-text) for a readable name. Passing this plugin's own `-1`/`-2`/`-3` codes into `ICU Get error text` isn't guaranteed to produce a meaningful string, since those aren't real ICU codes.
- `JIS Convert to text` and `JIS Convert from text` don't use that Longint convention at all — a failed conversion comes back as an **empty** Text/Blob, with no separate error code.
- `ICONV Convert`'s error code is the platform's raw C `errno` value (from libiconv), not one of this plugin's `-1`/`-2`/`-3` codes and not an ICU code.
- **On Windows**, the `CP *` and `JIS *` commands go through the `IMultiLanguage2` COM interface, which requires COM to already be initialized on the thread 4D dispatches the call on. If it isn't, these commands return `ERR_CONVERSION_FAILED`/`ERR_DETECTION_FAILED` (as of this build) instead of succeeding.
- **On macOS**, `CP Convert to text` falls back to the deprecated Carbon Text Encoding Conversion Manager only when the modern `CFStringCreateWithBytes` call fails for the requested codepage; this fallback is rarely exercised in practice.
- Bundles ICU (macOS 68.2 / Windows 61.1) and libiconv 1.14, per the project's README — encoding names for the `ICU *`/`ICONV *` commands follow ICU's and libiconv's own naming conventions (e.g. `"euc-jp-2007"`, `"utf-8"`), not 4D's `CONVERT FROM TEXT` codepage numbers.
- The three `*GET ENCODING LIST` commands and the encoding-guessing commands (`Get good encodings`/`Get best encoding`) can return **different result sets on Windows vs. macOS**, since each platform queries a different underlying system/library encoding table (the README reports ~147 platform-native encodings on Windows 7 vs. ~91 on macOS 10.10, for example).

---

## ICU GET ENCODING LIST

### Syntax
```
ICU GET ENCODING LIST ( arrEncodings )
```

| Parameter | Type | Description |
|---|---|---|
| `arrEncodings` | Array Text | Filled with the name of every ICU converter available on this machine |
| Result | — | No return value |

### Description
Reads a list ICU built once, at plugin-load time (`ucnv_countAvailable`/`ucnv_getAvailableName`), into `arrEncodings`. It doesn't re-scan ICU on every call. Use one of the returned names as the `encoding` parameter for [`ICU Convert to text`](#icu-convert-to-text) / [`ICU Convert from text`](#icu-convert-from-text).

### Example
From the plugin's own README:
```4d
ARRAY TEXT($names;0)

ICU GET ENCODING LIST ($names)
  //231 on both Windows 7 and OS X 10.10
```

---

## ICU Get good encodings

### Syntax
```
ICU Get good encodings ( blob ; arrNames ; arrLanguages ; arrConfidences ) -> Longint
```

| Parameter | Type | Description |
|---|---|---|
| `blob` | Blob | Byte data whose encoding you want to guess |
| `arrNames` | Array Text | Filled with every plausible encoding name ICU's charset detector found |
| `arrLanguages` | Array Text | Filled, in the same order as `arrNames`, with ICU's best-guess language for each match |
| `arrConfidences` | Array Longint | Filled, in the same order, with ICU's confidence score (0–100) for each match |
| Result | Longint | `0` on success; otherwise a raw ICU `UErrorCode` |

### Description
Wraps ICU's `ucsdet_detectAll`. `arrNames`, `arrLanguages`, and `arrConfidences` are always the same length and in the same order — index `i` of each array describes the same candidate match. If detection fails outright (for example, an empty `blob`), the return value carries the ICU error code and the array parameters aren't populated with any match data.

### Example
From the plugin's own README:
```4d
$err:=ICU Convert from text ("あいうえお";"euc-jp-2007";$euc)
$err:=ICU Get good encodings ($euc;$encodings;$languages;$confidences)
```

---

## ICU Get best encoding

### Syntax
```
ICU Get best encoding ( blob ; name ; language ; confidence ) -> Longint
```

| Parameter | Type | Description |
|---|---|---|
| `blob` | Blob | Byte data whose encoding you want to guess |
| `name` | Text | Filled with the single most likely encoding's name |
| `language` | Text | Filled with ICU's best-guess language for that match |
| `confidence` | Longint | Filled with the confidence score (0–100) for that match |
| Result | Longint | `0` on success; otherwise a raw ICU `UErrorCode` |

### Description
Wraps ICU's `ucsdet_detect` — the single best match, rather than [`ICU Get good encodings`](#icu-get-good-encodings)'s full ranked list.

### Example
```4d
$err:=ICU Convert from text ("あいうえお";"euc-jp-2007";$euc)
$err:=ICU Get best encoding ($euc;$name;$language;$confidence)
If ($err=0)
    ALERT($name+" ("+String($confidence)+"% confidence)")
End if 
```

---

## ICU Normalize text

### Syntax
```
ICU Normalize text ( text ; form ; normalized ) -> Longint
```

| Parameter | Type | Description |
|---|---|---|
| `text` | Text | Source text |
| `form` | Longint | Normalization form: `0`=NFC, `1`=NFD, `2`=NFKC, `3`=NFKD; any other value normalizes to NFKC_CF |
| `normalized` | Text | Filled with the normalized text |
| Result | Longint | `0` on success; otherwise a raw ICU `UErrorCode` |

### Description
Wraps ICU's `unorm2_normalize`. NFKC_CF isn't a distinct numbered option in this plugin's own interface — it's simply what you get from the `default` branch, i.e. any `form` value outside `0`–`3`.

### Example
From the plugin's own README:
```4d
$error:=ICU Normalize text ("ががが";ICU Normalize NFD;$normalized)
$length:=Length($normalized)
  //6; because NFD splits "が" to letters and combining accent characters
```
(`ICU Normalize NFD` here is a 4D constant your project must define matching this plugin's `1` — see the note under [`JIS Convert to text`](#jis-convert-to-text) for the same pattern. If your project doesn't define it, pass the literal `1`.)

---

## ICU Transform text

### Syntax
```
ICU Transform text ( id ; rules ; direction ; text ; transformed ) -> Longint
```

| Parameter | Type | Description |
|---|---|---|
| `id` | Text | ICU transliterator ID (e.g. `"Katakana-Hiragana"`, `"Latin-Cyrillic"`). Pass an empty string to use `rules` instead |
| `rules` | Text | Custom ICU transform rule syntax; only used when `id` is empty |
| `direction` | Longint | `0` = forward, `1` = reverse (ICU's `UTransDirection`) |
| `text` | Text | Source text |
| `transformed` | Text | Filled with the transformed text |
| Result | Longint | `0` on success; otherwise a raw ICU `UErrorCode` |

### Description
Wraps ICU's `utrans_openU`/`utrans_transUChars`. Either supply a built-in transliterator `id` (see the [ICU Transforms user guide](http://userguide.icu-project.org/transforms/general)) or leave `id` empty and supply your own `rules` string instead.

### Example
From the plugin's own README:
```4d
$src:="ﾊﾝｶｸゼンカクひらがな"
$id:="[:East Asian Width=Halfwidth:];Katakana-Hiragana"
$rule:=""
$error:=ICU Transform text ($id;$rule;ICU Transform Forward;$src;$dst)
  //はんかくゼンカクひらがな
```
```4d
$src:="それは12345ですか"
$id:=""
$rule:="[:^Number:] > ; # 数字だけを抜き取るぞ"
$error:=ICU Transform text ($id;$rule;ICU Transform Forward;$src;$dst)
  //12345
```
(`ICU Transform Forward` is a 4D constant your project must define matching this plugin's `0` — again, pass the literal `0` if your project doesn't define it.)

---

## ICU Get error text

### Syntax
```
ICU Get error text ( errorCode ) -> Text
```

| Parameter | Type | Description |
|---|---|---|
| `errorCode` | Longint | An ICU `UErrorCode`, as returned by one of the `ICU *` commands above |
| Result | Text | Human-readable ICU error name (e.g. `"U_ZERO_ERROR"`, `"U_INVALID_CHAR_FOUND"`) |

### Description
A thin wrapper over ICU's `u_errorName`. Only meaningful for codes actually produced by an `ICU *` command — see the note in [Requirements & platform notes](#requirements--platform-notes) about this plugin's own `-1`/`-2`/`-3` codes.

### Example
```4d
$err:=ICU Convert from text ("あいうえお";"not-a-real-encoding";$blob)
If ($err#0)
    ALERT(ICU Get error text ($err))
End if 
```

---

## ICU Convert to text

### Syntax
```
ICU Convert to text ( blob ; encoding ; text ) -> Longint
```

| Parameter | Type | Description |
|---|---|---|
| `blob` | Blob | Encoded byte data |
| `encoding` | Text | ICU converter name (from [`ICU GET ENCODING LIST`](#icu-get-encoding-list)) |
| `text` | Text | Filled with the decoded text |
| Result | Longint | `0` on success; otherwise a raw ICU `UErrorCode` |

### Description
Wraps ICU's `ucnv_open`/`ucnv_toUChars`.

### Example
From the plugin's own README:
```4d
$err:=ICU Convert from text ("あいうえお";"euc-jp-2007";$euc)
$err:=ICU Convert to text ($euc;"euc-jp-2007";$unicode)
```

---

## ICU Convert from text

### Syntax
```
ICU Convert from text ( text ; encoding ; blob ) -> Longint
```

| Parameter | Type | Description |
|---|---|---|
| `text` | Text | Source text |
| `encoding` | Text | ICU converter name (from [`ICU GET ENCODING LIST`](#icu-get-encoding-list)) |
| `blob` | Blob | Filled with the encoded byte data |
| Result | Longint | `0` on success; otherwise a raw ICU `UErrorCode` |

### Description
Wraps ICU's `ucnv_open`/`ucnv_fromUChars`. Guards internally against an oversized `text` overflowing the internal 32-bit buffer-size calculation — an input large enough to trigger that returns `ERR_CONVERSION_FAILED` (`-2`) rather than crashing.

### Example
From the plugin's own README and test method (`TEST.4dm`):
```4d
$err:=ICU Convert from text ("あいうえお";"euc-jp-2007";$EUC)
```

---

## ICONV Convert

### Syntax
```
ICONV Convert ( inEncoding ; outEncoding ; sourceBlob ; targetBlob ) -> Longint
```

| Parameter | Type | Description |
|---|---|---|
| `inEncoding` | Text | libiconv encoding name of `sourceBlob` |
| `outEncoding` | Text | libiconv encoding name to convert into |
| `sourceBlob` | Blob | Source byte data |
| `targetBlob` | Blob | Filled with the converted byte data |
| Result | Longint | `0` on success; otherwise the platform's raw `errno` from libiconv — **not** one of this plugin's `-1`/`-2`/`-3` codes and not an ICU code |

### Description
Wraps libiconv's `iconv_open`/`iconv`. Encoding names come from [`ICONV GET ENCODING LIST`](#iconv-get-encoding-list) (libiconv's own vocabulary, e.g. `"utf-8"`, `"euc-jp"` — not ICU names and not Windows codepage numbers).

### Example
From the plugin's own test method (`TEST.4dm`):
```4d
TEXT TO BLOB:C554("あいうえお"; $UTF8; UTF8 text without length:K22:17)
$err:=ICONV Convert("utf-8"; "euc-jp"; $UTF8; $EUC)
```

---

## JIS Convert to text

### Syntax
```
JIS Convert to text ( blob ; type ) -> Text
```

| Parameter | Type | Description |
|---|---|---|
| `blob` | Blob | JIS-variant-encoded byte data |
| `type` | Longint | `0` = SO/SI half-width katakana form, `2` = 8-bit (JIS8) katakana byte-range form |
| Result | Text | Decoded text, or an **empty** Text if decoding fails |

### Description
Converts the half-width-katakana encoding named by `type` into ISO-2022-JP bytes internally, then decodes those bytes with the platform's native ISO-2022-JP decoder (`NSISO2022JPStringEncoding` on macOS; MLang codepage 50221 on Windows).

The header defines a third constant, `JIS7_ESC` (`1`), for an escape-sequence-only variant — **it is currently a no-op in this build**: the internal pre-processing step only implements `type` `0` and `2`, so passing `1` skips that step entirely and hands the bytes straight to the platform decoder unmodified.

The README's own examples reference `JIS7_SO_SI`, `JIS7_ESC`, and `JIS8` as bare identifiers. These are 4D constants a project must define itself to match this header's `0`/`1`/`2` values — they aren't shipped in this plugin's `manifest.json`. If your project doesn't already define them, pass the literal numbers instead.

### Example
Adapted from the plugin's own README (using literal numbers in place of the project-defined constants):
```4d
$test1:=JIS Convert from text ("ｱ";1)  // JIS7_ESC
$result:=JIS Convert to text ($test1;1)
ALERT($result)

$test3:=JIS Convert from text ("ｱ";2)  // JIS8
$result:=JIS Convert to text ($test3;2)
ALERT($result)
```

---

## JIS Convert from text

### Syntax
```
JIS Convert from text ( text ; type ) -> Blob
```

| Parameter | Type | Description |
|---|---|---|
| `text` | Text | Source text |
| `type` | Longint | `0` = SO/SI half-width katakana form, `2` = 8-bit (JIS8) katakana byte-range form (same `JIS7_ESC` (`1`) no-op caveat as [`JIS Convert to text`](#jis-convert-to-text)) |
| Result | Blob | Encoded byte data, or an **empty** Blob if encoding fails |

### Description
The reverse of [`JIS Convert to text`](#jis-convert-to-text): encodes with the platform's native ISO-2022-JP encoder, then converts the ISO-2022-JP bytes into the half-width-katakana form named by `type`.

### Example
```4d
$blob:=JIS Convert from text ("漢字ｶﾀｶﾅabc漢字abcｶﾀｶﾅ";0)  // JIS7_SO_SI
$text:=JIS Convert to text ($blob;0)
ALERT($text)
```

---

## ICONV GET ENCODING LIST

### Syntax
```
ICONV GET ENCODING LIST ( arrEncodings )
```

| Parameter | Type | Description |
|---|---|---|
| `arrEncodings` | Array Text | Filled with the name of every libiconv encoding available on this machine |
| Result | — | No return value |

### Description
Filled once, at plugin-load time, via libiconv's `iconvlist`. Per the plugin's own README, this is ~345 entries on macOS 10.10 and ~393 on Windows 7. Use one of the returned names with [`ICONV Convert`](#iconv-convert).

### Example
From the plugin's own README:
```4d
ARRAY TEXT($names;0)

ICONV GET ENCODING LIST ($names)
  //393 on Windows 7 and 345 on OS X 10.10
```

---

## CP GET ENCODING LIST

### Syntax
```
CP GET ENCODING LIST ( arrCodes ; arrNames )
```

| Parameter | Type | Description |
|---|---|---|
| `arrCodes` | Array Longint | Filled with the Windows codepage number for each entry |
| `arrNames` | Array Text | Filled, in the same order as `arrCodes`, with the encoding/charset name for each entry |
| Result | — | No return value |

### Description
Filled once, at plugin-load time. **On Windows**, enumerated via `IMultiLanguage2::EnumCodePages` — if COM isn't available yet at plugin-load time, both arrays simply come back without any entries added, with no error reported (there's no return value on this command to report one through). **On macOS**, enumerated via `CFStringGetListOfAvailableEncodings` and translated to the nearest Windows codepage number via `_CFStringConvertEncodingToWindowsCodepage`; entries with no Windows-codepage equivalent are skipped. Use one of the returned codepage numbers with [`CP Convert from text`](#cp-convert-from-text) / [`CP Convert to text`](#cp-convert-to-text) / [`CP Get best encoding`](#cp-get-best-encoding) / [`CP Get good encodings`](#cp-get-good-encodings).

### Example
From the plugin's own README:
```4d
ARRAY LONGINT($codes;0)
ARRAY TEXT($names;0)

CP GET ENCODING LIST ($codes;$names)
  //147 on Win7 (JA) and 91 on OS X 10.10
```

---

## CP Get best encoding

### Syntax
```
CP Get best encoding ( blob ; codepage ) -> Longint
```

| Parameter | Type | Description |
|---|---|---|
| `blob` | Blob | Byte data whose encoding you want to guess |
| `codepage` | Longint | Filled with the Windows codepage number of the single best-matching encoding |
| Result | Longint | `0` on success; otherwise `ERR_DETECTION_FAILED` (`-3`) |

### Description
**On Windows**, wraps `IMultiLanguage2::DetectInputCodepage`, picking the candidate with the highest reported confidence. **On macOS**, wraps the Carbon `TECSniffTextEncoding` sniffer, picking the candidate with the fewest sniff errors (ties broken by the most recognized features), then translating it to the nearest Windows codepage via `TextEncodingNameToWindowsCodepage`.

### Example
```4d
$err:=CP Convert from text ("あいうえお";$euc;51932)
$err:=CP Get best encoding ($euc;$codepage)
If ($err=0)
    ALERT(String($codepage))
End if 
```

---

## CP Convert from text

### Syntax
```
CP Convert from text ( text ; blob ; codepage ) -> Longint
```

| Parameter | Type | Description |
|---|---|---|
| `text` | Text | Source text |
| `blob` | Blob | Filled with the encoded byte data |
| `codepage` | Longint | Windows codepage number (from [`CP GET ENCODING LIST`](#cp-get-encoding-list) or [`CP Get best encoding`](#cp-get-best-encoding)) |
| Result | Longint | `0` on success; `-1` (`ERR_INVALID_ENCODING`) if `codepage` isn't a valid/available encoding; `-2` (`ERR_CONVERSION_FAILED`) otherwise |

### Description
**On macOS**, translates `codepage` to a `CFStringEncoding` and uses `CFStringGetBytes`. **On Windows**, uses `IMultiLanguage2::ConvertStringFromUnicode`.

### Example
From the plugin's own test method (`TEST.4dm`):
```4d
$err:=CP Convert from text("あいうえお"; $EUC; 51932)
```

---

## CP Convert to text

### Syntax
```
CP Convert to text ( blob ; text ; codepage ) -> Longint
```

| Parameter | Type | Description |
|---|---|---|
| `blob` | Blob | Encoded byte data |
| `text` | Text | Filled with the decoded text |
| `codepage` | Longint | Windows codepage number |
| Result | Longint | `0` on success; `-1` (`ERR_INVALID_ENCODING`) if `codepage` isn't valid/available; `-2` (`ERR_CONVERSION_FAILED`) otherwise |

### Description
**On macOS**, translates `codepage` to a `CFStringEncoding` and uses `CFStringCreateWithBytes`, falling back to the deprecated Carbon Text Encoding Conversion Manager only if that fails. **On Windows**, uses `IMultiLanguage2::ConvertStringToUnicode`.

### Example
```4d
$err:=CP Convert from text ("あいうえお";$euc;51932)
$err:=CP Convert to text ($euc;$unicode;51932)
```

---

## CP Get good encodings

### Syntax
```
CP Get good encodings ( blob ; arrCodepages ) -> Longint
```

| Parameter | Type | Description |
|---|---|---|
| `blob` | Blob | Byte data whose encoding you want to guess |
| `arrCodepages` | Array Longint | Filled with the Windows codepage number of every plausible encoding |
| Result | Longint | `0` on success; otherwise `ERR_DETECTION_FAILED` (`-3`) |

### Description
**On Windows**, wraps `IMultiLanguage2::DetectInputCodepage`, collecting every candidate the API reports. **On macOS**, wraps the Carbon `TECSniffTextEncoding` sniffer, collecting every candidate with zero sniff errors (rather than stopping at the first non-matching one), then translating each to the nearest Windows codepage via `TextEncodingNameToWindowsCodepage`.

### Example
From the plugin's own README:
```4d
$err:=CP Convert from text ("あいうえお";$euc;51932)
$err:=CP Get good encodings ($euc;$codepages)
```

---

## Error handling & troubleshooting

- **A `0` return means success — but not every nonzero code means the same kind of failure.** `-1`/`-2`/`-3` are this plugin's own codes; any other value from an `ICU *` command is a raw ICU `UErrorCode` (decode it with [`ICU Get error text`](#icu-get-error-text)); `ICONV Convert`'s nonzero code is a raw `errno`, not an ICU code and not one of `-1`/`-2`/`-3`.
- **`JIS Convert to text`/`JIS Convert from text` fail silently.** They return content directly (Text/Blob), not a Longint — a failed conversion just comes back as empty. Check the result's length rather than expecting an error code.
- **`type` `1` (`JIS7_ESC`) currently does nothing.** Both JIS commands only implement `0` and `2`; passing `1` skips their internal pre/post-processing step and passes bytes straight through to/from the platform's native ISO-2022-JP codec.
- **Encoding names aren't interchangeable between command families.** ICU names ([`ICU GET ENCODING LIST`](#icu-get-encoding-list)), libiconv names ([`ICONV GET ENCODING LIST`](#iconv-get-encoding-list)), and Windows codepage numbers ([`CP GET ENCODING LIST`](#cp-get-encoding-list)) are three separate vocabularies — a name from one list won't mean anything to a command from a different family.
- **Windows `CP *`/`JIS *` commands can fail if COM isn't ready on the calling thread.** They depend on `IMultiLanguage2`, which requires COM already initialized on whichever thread 4D dispatches the call on. If it isn't, these commands report `ERR_CONVERSION_FAILED`/`ERR_DETECTION_FAILED` rather than silently returning an empty/zero result.
- **Encoding lists and guesses can differ between macOS and Windows** for the same input, since each platform queries its own system/library encoding table — verify on both platforms if your code needs to run cross-platform.
- **4D constants referenced in some examples above (`JIS7_SO_SI`, `JIS7_ESC`, `JIS8`, `ICU Normalize NFD`, `ICU Transform Forward`, etc.) aren't shipped in this plugin's `manifest.json`.** If your project hasn't defined them itself, pass the plugin header's literal numeric values shown alongside each in this doc.

---

## Quick reference

```4d
// ICU: encode/decode + guess
$err:=ICU Convert from text ("あいうえお";"euc-jp-2007";$blob)
$err:=ICU Convert to text ($blob;"euc-jp-2007";$text)
$err:=ICU Get best encoding ($blob;$name;$language;$confidence)

// libiconv: encode/decode between two iconv names
$err:=ICONV Convert ("utf-8";"euc-jp";$utf8;$euc)

// Windows codepage: encode/decode + guess
$err:=CP Convert from text ($text;$blob;51932)
$err:=CP Convert to text ($blob;$text;51932)
$err:=CP Get best encoding ($blob;$codepage)

// JIS half-width katakana (type: 0=SO/SI, 2=JIS8)
$blob:=JIS Convert from text ($text;0)
$text:=JIS Convert to text ($blob;0)

// Error text for any ICU-family nonzero result
If ($err#0)
    ALERT(ICU Get error text ($err))
End if 
```
