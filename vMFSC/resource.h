//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by DemoVersion.rc

// Next default values for new objects
//
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        101
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1001
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_REVIS 0
#define VERSION_BUILD 238

#define VERSION		VERSION_MAJOR,VERSION_MINOR,VERSION_REVIS,VERSION_BUILD
#define VERSION_STR STRINGIZE(VERSION_MAJOR) "." STRINGIZE(VERSION_MINOR) "." STRINGIZE(VERSION_REVIS) "." STRINGIZE(VERSION_BUILD)

#define PRODUCT_NAME "vMFSC"
#define ORIGINAL_FILENAME "vMFSC.exe"
#define FILE_DESCRIPTION "vMFSC"
#define COMPANY_NAME "Bach Nguyen Sy"

#define INTERNAL_NAME ORIGINAL_FILENAME
#define FILE_VERSION VERSION_STR
#define PRODUCT_VERSION VERSION_STR

// FILEVERSION VERSION
// PRODUCTVERSION VERSION

//VALUE "CompanyName",		COMPANY_NAME
//VALUE "FileDescription",	FILE_DESCRIPTION
//VALUE "FileVersion",		VERSION_STR
//VALUE "InternalName",		INTERNAL_NAME
//
//VALUE "OriginalFilename", ORIGINAL_FILENAME
//VALUE "ProductName",		PRODUCT_NAME
//VALUE "ProductVersion",	VERSION_STR

//Command line: AutoIncrementVersion --build $(ProjectDir)
//Description: Auto increment version