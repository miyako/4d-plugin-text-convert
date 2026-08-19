//%attributes = {}
CONVERT FROM TEXT:C1011("あいうえお"; "euc-jp"; $EUC)
$err:=CP Convert from text("あいうえお"; $EUC; 51932)
$err:=ICU Convert from text("あいうえお"; "euc-jp-2007"; $EUC)

TEXT TO BLOB:C554("あいうえお"; $UTF8; UTF8 text without length:K22:17)
$err:=ICONV Convert("utf-8"; "euc-jp"; $UTF8; $EUC)