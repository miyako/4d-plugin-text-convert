# 4d-plugin-text-convert

A plugin to perform various text convert operartions that are not currently support in 4D.

About
---
The native 4D commands **CONVERT FROM TEXT** and **Convert to text** supports various encodings, as listed [here](http://doc.4d.com/4Dv14R4/4D/14-R4/CONVERT-FROM-TEXT.301-1708428.en.html), but some encodings such as EUC-JP and ISO-2022-JP-1 are not supported.

The plugin provides interface to the ICU (5.4.1) and LIBICONV (1.14) libraries, as well as the platform native APIs that perform similar tasks.

In addition, there are commands to guess the possible encoding of a string, perform Unicode [Transform](http://userguide.icu-project.org/transforms/general) or apply [Normalization](http://userguide.icu-project.org/transforms/normalization).

Example
---
Get the list of applicable encodings.
```
ARRAY LONGINT($codes;0)
ARRAY TEXT($names;0)

CP GET ENCODING LIST ($codes;$names)
  //147 on Win7 (JA), 91 on OS X 10.10
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
