/****************************************************************************

    PROGRAM: Generic.c

    PURPOSE: Generic template for Windows applications

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	InitApplication() - initializes window data and registers window
	InitInstance() - saves instance handle and creates main window
	MainWndProc() - processes messages
	About() - processes messages for "About" dialog box

    COMMENTS:

        Windows can have several copies of your application running at the
        same time.  The variable hInst keeps track of which instance this
        application is so that processing will be to the correct window.

****************************************************************************/

#include "windows.h"		    /* required for all Windows applications */
#include "c_includ.h"
#include "generic.h"		    /* specific to this program		     */

HANDLE hInst;			    /* current instance			     */
HDC hdc;
LPSTR lpszCmdLine;
char **_argv;


/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: calls initialization function, processes message loop

    COMMENTS:

        Windows recognizes this function by name as the initial entry point 
        for the program.  This function calls the application initialization 
        routine, if no other instance of the program is running, and always 
        calls the instance initialization routine.  It then executes a message 
        retrieval and dispatch loop that is the top-level control structure 
        for the remainder of execution.  The loop is terminated when a WM_QUIT 
        message is received, at which time this function exits the application 
        instance by returning the value passed by PostQuitMessage(). 

        If this function must abort before entering the message loop, it 
        returns the conventional value NULL.  

****************************************************************************/

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;			     /* current instance	     */
HANDLE hPrevInstance;			     /* previous instance	     */
LPSTR lpCmdLine;			     /* command line		     */
int nCmdShow;				     /* show-window type (open/icon) */
{
    MSG msg;				     /* message			     */
    lpszCmdLine = lpCmdLine;

    if (!hPrevInstance)			 /* Other instances of app running? */
	if (!InitApplication(hInstance)) /* Initialize shared things */
	    return (FALSE);		 /* Exits if unable to initialize     */

    /* Perform initializations that apply to a specific instance */

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    /* Acquire and dispatch messages until a WM_QUIT message is received. */

    while (GetMessage(&msg,	   /* message structure			     */
	    NULL,		   /* handle of window receiving the message */
	    NULL,		   /* lowest message to examine		     */
	    NULL))		   /* highest message to examine	     */
	{
	TranslateMessage(&msg);	   /* Translates virtual key codes	     */
	DispatchMessage(&msg);	   /* Dispatches message to window	     */
    }
    return (msg.wParam);	   /* Returns the value from PostQuitMessage */
}


/****************************************************************************

    FUNCTION: InitApplication(HANDLE)

    PURPOSE: Initializes window data and registers window class

    COMMENTS:

        This function is called at initialization time only if no other 
        instances of the application are running.  This function performs 
        initialization tasks that can be done once for any number of running 
        instances.  

        In this case, we initialize a window class by filling out a data 
        structure of type WNDCLASS and calling the Windows RegisterClass() 
        function.  Since all instances of this application use the same window 
        class, we only need to do this when the first instance is initialized.  


****************************************************************************/

BOOL InitApplication(hInstance)
HANDLE hInstance;			       /* current instance	     */
{
    WNDCLASS  wc;

    /* Fill in window class structure with parameters that describe the       */
    /* main window.                                                           */

    wc.style = NULL;			 /* Class style(s).		       */
    wc.lpfnWndProc = (WNDPROC)MainWndProc;	 /* Function to retrieve messages for  */
                                        /* windows of this class.             */
    wc.cbClsExtra = 0;                  /* No per-class extra data.           */
    wc.cbWndExtra = 0;                  /* No per-window extra data.          */
    wc.hInstance = hInstance;           /* Application that owns the class.   */
    wc.hIcon = LoadIcon(hInstance, "TANK" );
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName =  "GenericMenu";   /* Name of menu resource in .RC file. */
    wc.lpszClassName = "GenericWClass"; /* Name used in call to CreateWindow. */

    /* Register the window class and return success/failure code. */

    return (RegisterClass(&wc));

}


/****************************************************************************

    FUNCTION:  InitInstance(HANDLE, int)

    PURPOSE:  Saves instance handle and creates main window

    COMMENTS:

        This function is called at initialization time for every instance of 
        this application.  This function performs initialization tasks that 
        cannot be shared by multiple instances.  

        In this case, we save the instance handle in a static variable and 
        create and display the main program window.  
        
****************************************************************************/

BOOL InitInstance(hInstance, nCmdShow)
    HANDLE          hInstance;          /* Current instance identifier.       */
    int             nCmdShow;           /* Param for first ShowWindow() call. */
{
    HWND            hWnd;               /* Main window handle.                */

    /* Save the instance handle in static variable, which will be used in  */
    /* many subsequence calls from this application to Windows.            */

    hInst = hInstance;

    /* Create a main window for this application instance.  */

    hWnd = CreateWindow(
        "GenericWClass",                /* See RegisterClass() call.          */
	"Windows BattleZone",	 /* Text for window title bar.	     */
        WS_OVERLAPPEDWINDOW,            /* Window style.                      */
	CW_USEDEFAULT,			/* Default horizontal position.       */
        CW_USEDEFAULT,                  /* Default vertical position.         */
        CW_USEDEFAULT,                  /* Default width.                     */
        CW_USEDEFAULT,                  /* Default height.                    */
        NULL,                           /* Overlapped windows have no parent. */
        NULL,                           /* Use the window class menu.         */
        hInstance,                      /* This instance owns this window.    */
        NULL                            /* Pointer not needed.                */
    );

    /* If window could not be created, return "failure" */

    if (!hWnd)
        return (FALSE);

    /* Make the window visible; update its client area; and return "success" */

    ShowWindow(hWnd, nCmdShow);  /* Show the window                        */
    UpdateWindow(hWnd);          /* Sends WM_PAINT message                 */
    return (TRUE);               /* Returns the value from PostQuitMessage */

}

/****************************************************************************

    FUNCTION: MainWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_COMMAND    - application menu (About dialog box)
	WM_DESTROY    - destroy window

    COMMENTS:

	To process the IDM_ABOUT message, call MakeProcInstance() to get the
	current instance address of the About() function.  Then call Dialog
	box which will create the box according to the information in your
	generic.rc file and turn control over to the About() function.	When
	it returns, free the intance address.

****************************************************************************/

long FAR PASCAL MainWndProc(hWnd, message, wParam, lParam)
HWND hWnd;				  /* window handle		     */
unsigned message;			  /* type of message		     */
UINT wParam;				  /* additional information	     */
LONG lParam;				  /* additional information	     */
{
    FARPROC lpProcAbout;		  /* pointer to the "About" function */
    MSG msg;
    RECT rect;

    switch (message) {
	case WM_CREATE:
	    hdc = GetDC( hWnd );
	    SetMapMode( hdc, MM_ISOTROPIC );
            break;

	case WM_SIZE:
	    GetClientRect( hWnd, &rect );
	    SetWindowExtEx( hdc, 1000, 700, NULL );
	    SetViewportExtEx( hdc, LOWORD(lParam), HIWORD(lParam), NULL );
	    break;

	case WM_COMMAND:	   /* message: command from application menu */
	    switch( wParam ) {
		case IDM_ORIGINAL:
		    opt->original = True;
		    opt->copters = False;
		    opt->mmissiles = 1;
		    opt->msalvos = 1;
		    MessageBeep( -1 );
		    cbzone_main( 0, NULL );
		    MessageBeep( -1 );
		    do {
		    } while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ));
		    InvalidateRect( hWnd, NULL, TRUE );
		    PostMessage( hWnd, WM_DESTROY, 0, 0 );
		    break;
		case IDM_COPT2:
		    opt->original = False;
		    opt->mmissiles = 1;
		    opt->msalvos = 10;
		    MessageBeep( -1 );
		    cbzone_main( 0, NULL );
		    MessageBeep( -1 );
		    do {
		    } while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ));
		    InvalidateRect( hWnd, NULL, TRUE );
		    PostMessage( hWnd, WM_DESTROY, 0, 0 );
		    break;
		case IDM_TANKCOPT:
		    opt->copters = True;
		    opt->mmissiles = 1;
		    opt->msalvos = 3;
		    MessageBeep( -1 );
		    cbzone_main( 0, NULL );
		    MessageBeep( -1 );
		    do {
		    } while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ));
		    InvalidateRect( hWnd, NULL, TRUE );
		    PostMessage( hWnd, WM_DESTROY, 0, 0 );
		    break;
		case IDM_ABOUT:
		    lpProcAbout = MakeProcInstance((FARPROC)About, hInst);

		    DialogBox(hInst,		 /* current instance	     */
			"AboutBox",			 /* resource to use	     */
			hWnd,			 /* parent handle	     */
			(WNDPROC)lpProcAbout);		 /* About() instance address */

		    FreeProcInstance(lpProcAbout);
		    break;
		default:
		    return (DefWindowProc(hWnd, message, wParam, lParam));
	    }
	    return 0;
	    break;

	case WM_DESTROY:		  /* message: window being destroyed */
	    ReleaseDC( hWnd, hdc );
	    PostQuitMessage(0);
	    break;

	default:			  /* Passes it on if unproccessed    */
	    return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}


/****************************************************************************

    FUNCTION: About(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for "About" dialog box

    MESSAGES:

	WM_INITDIALOG - initialize dialog box
	WM_COMMAND    - Input received

    COMMENTS:

	No initialization is needed for this particular dialog box, but TRUE
	must be returned to Windows.

	Wait for user to click on "Ok" button, then close the dialog box.

****************************************************************************/

BOOL FAR PASCAL About(hDlg, message, wParam, lParam)
HWND hDlg;                                /* window handle of the dialog box */
unsigned message;                         /* type of message                 */
UINT wParam;                              /* message-specific information    */
LONG lParam;
{
    switch (message) {
	case WM_INITDIALOG:		   /* message: initialize dialog box */
	    return (TRUE);

	case WM_COMMAND:		      /* message: received a command */
	    if (LOWORD(wParam) == IDOK                /* "OK" box selected?	     */
                || LOWORD(wParam) == IDCANCEL) {      /* System menu close command? */
		EndDialog(hDlg, TRUE);	      /* Exits the dialog box	     */
		return (TRUE);
	    }
	    break;
    }
    return (FALSE);			      /* Didn't process a message    */
}
