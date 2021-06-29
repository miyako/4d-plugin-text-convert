#include "cpmap.h"

CFStringEncoding _CFStringConvertWindowsCodepageToEncoding(UInt32 codepage)
{
	
	CFStringEncoding encoding = kCFStringEncodingInvalidId;
	
	switch(codepage)
	{
		case 20932 :
		case 51932 :            
			encoding = kCFStringEncodingEUC_JP;
			break;
			
		case 12000 :
		case 65005 :			
			encoding = kCFStringEncodingUTF32LE;
			break;	
			
		case 12001 :
		case 65006 :			
			encoding = kCFStringEncodingUTF32BE;
			break;	
			
		case 50220 :
			encoding = kCFStringEncodingISO_2022_JP;
			break;	
			
		case 923 :
			encoding = kCFStringEncodingISOLatin9;
			break;	
			
		case 1200 :
			encoding =  kCFStringEncodingUTF16LE;
			break;	
			
		case 1201 :
			encoding =  kCFStringEncodingUTF16BE;
			break;	
			
		case 65001 :
			encoding =  kCFStringEncodingUTF8;
			break;	
			
		case 720 :
			encoding =  kCFStringEncodingDOSArabic;
			break;				
			
		case 853 :
			encoding =  kCFStringEncodingISOLatin3;
			break;
			
		case 856 :
			encoding =  kCFStringEncodingDOSHebrew;
			break;
			
		case 951 :
			encoding =  kCFStringEncodingBig5_HKSCS_1999;
			break;
			
		case 38598 :
			encoding =  kCFStringEncodingISOLatinHebrew;
			break;
			
		case 57002 :
			encoding =  kCFStringEncodingMacDevanagari;
			break;
			
		case 57003 :
			encoding =  kCFStringEncodingMacBengali;
			break;
			
		case 57004 :
			encoding =  kCFStringEncodingMacTamil;
			break;
			
		case 57005 :
			encoding =  kCFStringEncodingMacTelugu;
			break;
			
		case 57007 :
			encoding =  kCFStringEncodingMacOriya;
			break;
			
		case 57008 :
			encoding =  kCFStringEncodingMacKannada;
			break;
			
		case 57009 :
			encoding =  kCFStringEncodingMacMalayalam;
			break;

		case 58060 :
			encoding =  kCFStringEncodingMacFarsi;
			break;
			
		case 58950 :
			encoding =  kCFStringEncodingEUC_TW;
			break;

		case 58010 :
			encoding =  kCFStringEncodingKOI8_U;
			break;

        case 932 :
            encoding =  kCFStringEncodingShiftJIS_X0213;
            break;
            
		default :
			encoding = CFStringConvertWindowsCodepageToEncoding(codepage);
			break;	
			
	}
    
	return encoding;
}

UInt32 _CFStringConvertEncodingToWindowsCodepage(CFStringEncoding encoding)
{
	UInt32 codepage = 0;
	
	switch( encoding )
	{
		case kCFStringEncodingEUC_JP :
			codepage = 51932;
			break;
			
		case kCFStringEncodingUTF32LE :
			codepage = 12000;
			break;			
			
		case kCFStringEncodingUTF32BE :
			codepage = 12001;
			break;	
			
		case kCFStringEncodingISO_2022_JP :
			codepage = 50220;
			break;	
			
        case kCFStringEncodingISO_2022_JP_1 :              
        case kCFStringEncodingISO_2022_JP_2 :            
        case kCFStringEncodingISO_2022_JP_3 :
            codepage = 0;
            break;	
            
		case kCFStringEncodingISOLatin9 :
			codepage = 923;
			break;	
			
		case kCFStringEncodingUTF16LE :
			codepage = 1200;
			break;
			
		case kCFStringEncodingUTF16BE :
			codepage = 1201;
			break;
			
		case kCFStringEncodingUTF8 :
			codepage = 65001;
			break;			
			
		case kCFStringEncodingDOSArabic :
			codepage = 720;
			break;	
			
		case kCFStringEncodingISOLatin3 :
			codepage = 853;
			break;
			
		case kCFStringEncodingDOSHebrew :
			codepage = 856;
			break;

		case kCFStringEncodingBig5 :
			codepage = 950;
			break;	
			
		case kCFStringEncodingBig5_HKSCS_1999 :
			codepage = 951;
			break;
			
		case kCFStringEncodingISOLatinHebrew :
			codepage = 38598;
			break;				
			
		case kCFStringEncodingShiftJIS :	
		case kCFStringEncodingShiftJIS_X0213 :	            
        //case kCFStringEncodingShiftJIS_X0213_00 :
			codepage = 932;
			break;					
			
		case kCFStringEncodingGBK_95 :
		case kCFStringEncodingDOSChineseSimplif :	
		case kCFStringEncodingMacChineseSimp :
			codepage = 936;
			break;	
			
		case kCFStringEncodingMacDevanagari :
			codepage = 57002;
			break;	
			
		case kCFStringEncodingMacBengali :
			codepage = 57003;
			break;	
			
		case kCFStringEncodingMacTamil :
			codepage = 57004;
			break;				

		case kCFStringEncodingMacTelugu :
			codepage = 57005;
			break;	
			
		case kCFStringEncodingMacOriya :
			codepage = 57007;
			break;	
			
		case kCFStringEncodingMacKannada :
			codepage = 57008;
			break;
			
		case kCFStringEncodingMacMalayalam :
			codepage = 57009;
			break;

		case kCFStringEncodingMacFarsi :
			codepage = 58060;
			break;
			
		case kCFStringEncodingMacGurmukhi :
		case kCFStringEncodingMacGujarati :
		case kCFStringEncodingMacTibetan :
		case kCFStringEncodingMacSymbol :
		case kCFStringEncodingMacDingbats :
		case kCFStringEncodingMacCeltic :
		case kCFStringEncodingMacGaelic :
		case kCFStringEncodingMacInuit :			
			codepage = 0;
			break;		
			
		case kCFStringEncodingMacRomanLatin1 :
		case kCFStringEncodingNextStepLatin :			
			codepage = 10000;
			break;	

		case kCFStringEncodingEUC_TW :
			codepage = 58950;
			break;

		case kCFStringEncodingKOI8_U :
			codepage = 58010;
			break;
			
		default :
			codepage = CFStringConvertEncodingToWindowsCodepage(encoding);
			break;	
			
	}
	
	return codepage;
	
}

TextEncoding TECConvertWindowsCodepageToTextEncoding(UInt32 codepage)
{
	
	TextEncoding encoding = kTextEncodingUnknown;
	
	switch(codepage)
	{
		case 20932 :
		case 51932 :            
			encoding = kTextEncodingEUC_JP;
			break;
			
		case 12000 :
			break;	
			
		case 12001 :
			break;	
			
		case 50220 :
			encoding = kTextEncodingISO_2022_JP;
			break;	
			
		case 923 :
			encoding = kTextEncodingISOLatin9;
			break;	
			
		case 1200 :
			break;	
			
		case 1201 :
			break;	
			
		case 65001 :
			encoding =  kUnicodeUTF8Format;
			break;	
			
		case 720 :
			encoding =  kTextEncodingDOSArabic;
			break;				
			
		case 853 :
			encoding =  kTextEncodingISOLatin3;
			break;
			
		case 856 :
			encoding =  kTextEncodingDOSHebrew;
			break;
            
        case 932 :
            encoding =  kTextEncodingShiftJIS_X0213_00;
            break;            
			
		case 951 :
			encoding =  kTextEncodingBig5_HKSCS_1999;
			break;
			
		case 38598 :
			encoding =  kTextEncodingISOLatinHebrew;
			break;
			
		case 57002 :
			encoding =  kTextEncodingMacDevanagari;
			break;
			
		case 57003 :
			encoding =  kTextEncodingMacBengali;
			break;
			
		case 57004 :
			encoding =  kTextEncodingMacTamil;
			break;
			
		case 57005 :
			encoding =  kTextEncodingMacTelugu;
			break;
			
		case 57007 :
			encoding =  kTextEncodingMacOriya;
			break;
			
		case 57008 :
			encoding =  kTextEncodingMacKannada;
			break;
			
		case 57009 :
			encoding =  kTextEncodingMacMalayalam;
			break;
			
		case 58060 :
			encoding =  kTextEncodingMacFarsi;
			break;
			
		case 58950 :
			encoding =  kTextEncodingEUC_TW;
			break;
			
		case 58010 :
			break;
			
		default :
			break;	
			
	}
	
	return encoding;
}

UInt32 TextEncodingNameToWindowsCodepage(CFStringRef name)
{
	UInt32 codepage = 0;
	UInt32 i = 0;
	
	const CFStringEncoding *encodings = CFStringGetListOfAvailableEncodings();

	while (encodings[i] != kCFStringEncodingInvalidId)
	{		
		if(CFStringCompare(CFStringGetNameOfEncoding(encodings[i]), name, 0) == kCFCompareEqualTo)
		{
			codepage = _CFStringConvertEncodingToWindowsCodepage(encodings[i]);
			break;
		}
		i++;
	}
	return codepage;	
}