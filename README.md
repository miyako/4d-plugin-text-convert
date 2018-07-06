# 4d-plugin-text-convert

A plugin to perform various text convert operartions that are not currently support in 4D

based on [iconv](https://www.gnu.org/software/libiconv/) and [ICU](http://site.icu-project.org/home)

LIBJPNとの違いについては[blog](http://www.4d.com/jp/blog/text-convert.html)を参照してください

### Platform

| carbon | cocoa | win32 | win64 |
|:------:|:-----:|:---------:|:---------:|
|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|

### Version

<img src="https://cloud.githubusercontent.com/assets/1725068/18940649/21945000-8645-11e6-86ed-4a0f800e5a73.png" width="32" height="32" /> <img src="https://cloud.githubusercontent.com/assets/1725068/18940648/2192ddba-8645-11e6-864d-6d5692d55717.png" width="32" height="32" /> <img src="https://user-images.githubusercontent.com/1725068/41266195-ddf767b2-6e30-11e8-9d6b-2adf6a9f57a5.png" width="32" height="32" />

About
---
The native 4D commands **CONVERT FROM TEXT** and **Convert to text** supports various encodings, as listed [here](http://doc.4d.com/4Dv14R4/4D/14-R4/CONVERT-FROM-TEXT.301-1708428.en.html), but some encodings such as EUC-JP and ISO-2022-JP-1 are not supported.

The plugin provides interface to the ICU (5.4.1) and LIBICONV (1.14) libraries, as well as the platform native APIs that perform similar tasks.

In addition, there are commands to guess the possible encoding of a string, perform Unicode [Transform](http://userguide.icu-project.org/transforms/general) or apply [Normalization](http://userguide.icu-project.org/transforms/normalization).

### Releases

![preemption xx](https://user-images.githubusercontent.com/1725068/41327179-4e839948-6efd-11e8-982b-a670d511e04f.png)

[2.0](https://github.com/miyako/4d-plugin-text-convert/releases/tag/2.0) static library

[1.1](https://github.com/miyako/4d-plugin-text-convert/releases/tag/1.1) dynamic library

### Examples

* Get the list of applicable encodings

```
ARRAY LONGINT($codes;0)
ARRAY TEXT($names;0)

CP GET ENCODING LIST ($codes;$names)
  //147 on Win7 (JA) and 91 on OS X 10.10
```

```
ARRAY TEXT($names;0)

ICU GET ENCODING LIST ($names)
  //231 on both Windows 7 and OS X 10.10
```

```
ARRAY TEXT($names;0)

ICONV GET ENCODING LIST ($names)
  //393 on Windows 7 and 345 on OS X 10.10
```

* Convert to EUC-JP and back

```
$err:=CP Convert from text ("あいうえお";$euc;51932)
$err:=CP Convert to text ($euc;$unicode;51932)
```

```
$err:=ICU Convert from text ("あいうえお";"euc-jp-2007";$euc)
$err:=ICU Convert to text ($euc;"euc-jp-2007";$unicode)
```

```
TEXT TO BLOB("あいうえお";$utf8;UTF8 text without length)
$err:=ICONV Convert ("utf-8";"euc-jp";$utf8;$euc)
$err:=ICONV Convert ("euc-jp";"utf-8";$euc;$utf8)
$original:=BLOB to text($utf8;UTF8 text without length)
```

JIS 
---

Various wayst to encode JIS (not to be confused with ISO-2022-JP).

```
$test1:=JIS Convert from text ("ｱ";JIS7_ESC)
$result:=JIS Convert to text ($test1;JIS7_ESC)
ALERT($result)

$test2:=JIS Convert from text ("ｱ";JIS7_SO_SI)
$result:=JIS Convert to text ($test1;JIS7_SO_SI)
ALERT($result)

$test3:=JIS Convert from text ("ｱ";JIS8)
$result:=JIS Convert to text ($test1;JIS8)
ALERT($result)

$test4:=JIS Convert from text ("漢字ｶﾀｶﾅabc漢字abcｶﾀｶﾅ";JIS8)
$result:=JIS Convert to text ($test4;JIS8)
ALERT($result)
```

ICU
---

* Transliteration

```
$src:="ﾊﾝｶｸゼンカクひらがな"
$id:="[:East Asian Width=Halfwidth:];Katakana-Hiragana"
$rule:=""
$error:=ICU Transform text ($id;$rule;ICU Transform Forward;$src;$dst)
  //はんかくゼンカクひらがな
```

```
$src:="Kaloudis"
$id:="Latin-Cyrillic"
$rule:=""
$error:=ICU Transform text ($id;$rule;ICU Transform Forward;$src;$dst)
  //Калоудис
```

```
$src:="株式会社４Ｄジャパン渋谷区道玄坂１ー１０ー２"
$id:="[:^Katakana:];Fullwidth-Halfwidth"
$rule:=""
  //株式会社4Dジャパン渋谷区道玄坂1ｰ10ｰ2
```

```
$src:="カキクケコ"
$id:="Katakana-Hiragana"
$rule:=""
$error:=ICU Transform text ($id;$rule;ICU Transform Forward;$src;$dst)
  //かきくけこ
```

```
$src:="それは12345ですか"
$id:=""
$rule:="[:^Number:] > ; # 数字だけを抜き取るぞ"
$error:=ICU Transform text ($id;$rule;ICU Transform Forward;$src;$dst)
  //12345
```

```
$src:="abcde"
$id:=""
$rule:="a > A; # aをAに変換するだけだ"
$error:=ICU Transform text ($id;$rule;ICU Transform Forward;$src;$dst)
  //Abcde
```

```
$src:="トランスフォームデスネ"
$id:="-Latin"
$rule:=""
$error:=ICU Transform text ($id;$rule;ICU Transform Forward;$src;$dst)
  //toransufōmudesune
```

* Normalization

```
$error:=ICU Normalize text ("ががが";ICU Normalize NFD;$normalized)
$length:=Length($normalized)
  //6; because NFD splits "が" to letters and combining accent characters
```

* Detect

```
$err:=ICU Convert from text ("あいうえお";"euc-jp-2007";$euc)
$err:=ICU Get good encodings ($euc;$encodings;$languages;$confidences)
```

```
$err:=CP Convert from text ("あいうえお";$euc;51932)
$err:=CP Get good encodings ($euc;$codepages)
```
