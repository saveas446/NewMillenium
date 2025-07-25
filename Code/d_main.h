
// d_main.h : game startup, and main loop code, system specific

#ifndef __D_MAIN__
#define __D_MAIN__

#ifdef _WIN32
typedef unsigned int mode_t;
#endif


#include "d_event.h"
#include "w_wad.h"   // for MAX_WADFILES


//extern char*        startupwadfiles[MAX_WADFILES];


//void D_AddFile (char *file);

// make sure not to write back the config until it's been correctly loaded
extern ULONG      rendergametic;

// the infinite loop of D_SRB2Loop() called from win_main for windows version
void D_DoomInnerLoop (void);

//
// D_SRB2Main()
// Not a globally visible function, just included for source reference,
// calls all startup code, parses command line options.
// If not overrided by user input, calls N_AdvanceDemo.
//
void D_SRB2Main (void);

// Called by IO functions when input is detected.
void D_PostEvent (event_t* ev);
void D_PostEvent_end (void);    // delimiter for locking memory

void D_ProcessEvents (void);
void D_DoAdvanceDemo (void);

#define MAX_DEVMAPNUM 31
extern char* wad_filenames[MAX_DEVMAPNUM];


//
// BASE LEVEL
//

// Just incase I need this in another .c file
extern int access_case_insensitive(const char *pathname, mode_t mode);


void D_PageTicker (void);
// pagename is lumpname of a 320x200 patch to fill the screen
void D_PageDrawer (char* pagename);
void D_AdvanceDemo (void);
void D_StartTitle (void);

#endif //__D_MAIN__
