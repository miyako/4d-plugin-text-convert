#ifndef __CPMAP_H__
#define __CPMAP_H__ 1

#ifdef __cplusplus
extern "C" {
#endif
    
#import <CoreFoundation/CoreFoundation.h>
#import <CoreServices/CoreServices.h>

// CFStringConvertWindowsCodepageToEncoding + missing encoding/codepages
CFStringEncoding _CFStringConvertWindowsCodepageToEncoding(UInt32 codepage);

// CFStringConvertEncodingToWindowsCodepage + missing encoding/codepages
UInt32 _CFStringConvertEncodingToWindowsCodepage(CFStringEncoding encoding);

// for use with ConvertFromTextToUnicode/TECConvertText, as a fallback to CFStringCreateWithBytes
TextEncoding TECConvertWindowsCodepageToTextEncoding(UInt32 codepage);

// assuming that GetTextEncodingName::kTextEncodingFullName is equal to CFStringGetNameOfEncoding
UInt32 TextEncodingNameToWindowsCodepage(CFStringRef name);

#ifdef __cplusplus
}
#endif

#endif
