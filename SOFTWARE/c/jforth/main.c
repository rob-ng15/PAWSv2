/**
@file       main.c
@author     Richard James Howe.
@copyright  Copyright 2015,2016,2017 Richard James Howe.
@license    MIT
@email      howe.r.j.89@gmail.com
**/
#include "libforth.h"
#include "unit.h"
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
extern int _fileno(FILE*);
#endif

#ifdef USE_BUILT_IN_CORE
extern unsigned char forth_core_data[];
extern forth_cell_t forth_core_size;
#endif

/**
Although multiple instances of a libforth environment can be active in a single
C application, this test program only has one active. This is stored in a
global variable so signal handlers can access it.
**/
static forth_t *global_forth_environment;
static int enable_signal_handling;

typedef void (*signal_handler)(int sig); /**< functions for handling signals*/

#ifdef USE_ABORT_HANDLER
#ifdef __unix__

#include <execinfo.h>
#define TRACE_SIZE      (64u)

/**
This hander calls functions (backtrace, printf) that are not
safe to call from a signal handler, however this is only going to
be called in the event of an internal consistency failure,
and only as a courtesy to the programmer.

A windows version could be made using information from:
https://msdn.microsoft.com/en-us/library/windows/desktop/bb204633%28v=vs.85%29.aspx and
https://stackoverflow.com/questions/5693192/win32-backtrace-from-c-code
**/
static void sig_abrt_handler(int sig)
{
        void *trace[TRACE_SIZE];
        char **messages = NULL;
        int i, trace_size;
	signal(sig, SIG_DFL);
        trace_size = backtrace(trace, TRACE_SIZE);
        messages = backtrace_symbols(trace, trace_size);
        if (trace_size < 0)
                goto fail;
        fprintf(stderr, "SIGABRT was raised.\nStack trace:\n");
        for (i = 0; i < trace_size; i++)
                fprintf(stderr, "\t%s\n", messages[i]);
        fflush(stderr);
fail:
        abort();
}
#endif
#endif

#ifdef USE_LINE_EDITOR
#include "libline.h"
#define LINE_EDITOR_AVAILABLE (1) /**< line editor is available */

/**
The Forth history file will be stored in this file, if the
**USE\_LINE\_EDITOR** option is set.
**/
static const char *history_file = ".forth";

/**
The line editor, if used, will print a prompt for each line:
**/
static const char *prompt = "> ";

/**
This is the line completion callback
**/
void forth_line_completion_callback(const char *line, size_t pos, line_completions *lc)
{
	size_t length = 0;
	char **s;
	assert(line);
	assert(lc);
	assert(global_forth_environment);
	s = forth_words(global_forth_environment, &length);
	(void)pos;
	if (!s)
		return;
	for (size_t i = 0; i < length; i++) {
		line_add_completion(lc, s[i]);
	}
	forth_free_words(s, length);
}

/**
@brief The following function implements a line-editor loop, quiting
when there is no more input to be read.
@param  o    a fully initialized for environment
@param  mode set vi mode on or off
@return int <0 on failure of the Forth execution or the line editor
**/
static int forth_line_editor(forth_t *o, int mode)
{
	int rval = 0;
	char *line = NULL;
	assert(o);
	line_set_vi_mode(mode);
	errno = 0;
	if (line_history_load(history_file) < 0) /* loading can fail, which is fine */
		warning("failed to load history file %s, %s", history_file, forth_strerror());
	line_set_completion_callback(forth_line_completion_callback);
	while ((line = line_editor(prompt))) {
		forth_set_string_input(o, line);
		if ((rval = forth_run(o)) < 0)
			goto end;
		if (line_history_add(line) < 0) {
			rval = -1;
			goto end;
		}
		if (line_history_save(history_file) < 0) {
			rval = -1;
			goto end;
		}
		free(line);
		line = NULL;
	}
end:
	free(line);
	return rval;
}
#else
#define LINE_EDITOR_AVAILABLE (0) /**< line editor is not available */
#endif /* USE_LINE_EDITOR */

static void register_signal_handler(int sig, signal_handler handler)
{
	errno = 0;
	if (signal(SIGINT, handler) == SIG_ERR) {
		error("could not install %d handler: %s", sig, forth_strerror());
		exit(EXIT_FAILURE);
	}
}

static void sig_generic_handler(int sig)
{
	if (enable_signal_handling) {
		forth_signal(global_forth_environment, sig);
		register_signal_handler(sig, sig_generic_handler);
	} else {
		exit(EXIT_FAILURE);
	}
}

/**
This program can be used as a filter in a Unix pipe chain, or as a standalone
interpreter for Forth. It tries to follow the Unix philosophy and way of
doing things (see <http://www.catb.org/esr/writings/taoup/html/ch01s06.html>
and <https://en.wikipedia.org/wiki/Unix_philosophy>). Whether this is
achieved is a matter of opinion. There are a things this interpreter does
differently to most Forth interpreters that support this philosophy however,
it is silent by default and does not clutter up the output window with "ok",
or by printing a banner at start up (which would contain no useful information
whatsoever). It is simple, and only does one thing (but does it do it well?).
**/
static void fclose_input(FILE **in)
{
	if (*in && (*in != stdin))
		fclose(*in);
	*in = stdin;
}

/**
It is customary for Unix programs to have a usage string, which we
can print out as a quick reminder to the user as to what the command
line options are.
**/
static void usage(const char *name)
{
	fprintf(stderr,
		"usage: %s "
		"[-(s|l|f) file] [-e expr] [-m size] [-LSVthvnx] [-] files\n",
		name);
}

/**
We try to keep the interface to the example program as simple as possible,
so there are limited, uncomplicated options. What they do
should come as no surprise to an experienced Unix programmer, it is important
to pick option names that they would expect (for example *-l* for loading,
*-e* for evaluation, and not using *-h* for help would be a hanging offense).
**/

static void help(void)
{
	static const char help_text[] =
"Forth: A small forth interpreter build around libforth\n\n"
"\t-h        print out this help and exit unsuccessfully\n"
"\t-u        run the built in unit tests, then exit\n"
"\t-e string evaluate a string\n"
"\t-s file   save state of forth interpreter to file\n"
"\t-S        save state to 'forth.core'\n"
"\t-n        use the line editor, if available, when reading from stdin\n"
"\t-f file   immediately read from and execute a file\n"
"\t-l file   load previously saved state from file\n"
"\t-L        load previously saved state from 'forth.core'\n"
"\t-m size   specify forth memory size in KiB (cannot be used with '-l')\n"
"\t-t        process stdin after processing forth files\n"
"\t-v        turn verbose mode on\n"
"\t-x        enable signal handling\n"
"\t-V        print out version information and exit\n"
"\t-         stop processing options\n\n"
"Options must come before files to execute.\n\n"
"The following words are built into the interpreter:\n\n";

	fputs(help_text, stderr);
}

static int eval_file(forth_t *o, const char *file, enum forth_debug_level verbose) {
	// EXPAND filename to include /COMPUTER/JFORTH/
	char path[1024];
	sprintf( path, "/COMPUTER/JFORTH/%s",file);
	FILE *in = NULL;
	int c = 0, rval = 0;
	//assert(file);
	if (verbose >= FORTH_DEBUG_NOTE)
		note("reading from file '%s'", path);
	forth_set_file_input(o, in = forth_fopen_or_die(path, "rb"));
	/* shebang line '#!', core files could also be detected */
	if ((c = fgetc(in)) == '#')
		while (((c = fgetc(in)) > 0) && (c != '\n'));
	else if (c == EOF)
		goto close;
	else
		ungetc(c, in);
	rval = forth_run(o);
close:
	fclose_input(&in);
	return rval;
}

static void version(void)
{
	fprintf(stdout,
		"libforth:\n"
		"\tversion:     %u\n"
		"\tsize:        %u\n"
		"\tendianess:   %u\n",
		FORTH_CORE_VERSION,
		(unsigned)sizeof(forth_cell_t) * CHAR_BIT,
		(unsigned)IS_BIG_ENDIAN);
}

static forth_t *forth_initial_enviroment(forth_t **o, forth_cell_t size,
		FILE *input, FILE *output, enum forth_debug_level verbose,
		int argc, char **argv)
{
	errno = 0;
	//assert(input && output && argv);
	if (*o)
		goto finished;

#ifdef USE_BUILT_IN_CORE
	/* USE_BUILT_IN_CORE is an experimental feature, it should not be
	 * relied upon to work correctly */
	(void)size;
	*o = forth_load_core_memory((char*)forth_core_data, forth_core_size);
	forth_set_file_input(*o, input);
	forth_set_file_output(*o, output);
#else
	*o = forth_init(size, input, output, NULL);
#endif
	if (!(*o)) {
		fatal("forth initialization failed, %s", forth_strerror());
		exit(EXIT_FAILURE);
	}

finished:
	forth_set_debug_level(*o, verbose);
	forth_set_args(*o, argc, argv);
	global_forth_environment = *o;
	return *o;
}

/**

To keep things simple options are parsed first then arguments like files,
although some options take arguments immediately after them.

A library for parsing command line options like *getopt* should be used,
this would reduce the portability of the program. It is not recommended
that arguments are parsed in this manner.

**/
int main(int argc, char **argv)
{
	FILE *in = NULL, *dump = NULL;
	int rval = 0, i = 1;
       	int save = 0,            /* attempt to save core if true */
	    eval = 0,            /* have we evaluated anything? */
	    readterm = 1,        /* read from standard in */
	    use_line_editor = 0, /* use a line editor, *if* one exists */
	    mset = 0;            /* memory size specified */
	enum forth_debug_level verbose = FORTH_DEBUG_OFF; /* verbosity level */
	static const size_t kbpc = 1024 / sizeof(forth_cell_t); /*kilobytes per cell*/
	static const char *dump_name = "forth.core";
	char *optarg = NULL;
	forth_cell_t core_size = DEFAULT_CORE_SIZE;
	forth_t *o = NULL;
	int orig_argc = argc;
	char **orig_argv = argv;

	register_signal_handler(SIGINT, sig_generic_handler);

#ifdef USE_ABORT_HANDLER
#ifdef __unix__
	register_signal_handler(SIGABRT, sig_abrt_handler);
#endif
#endif

#ifdef _WIN32
	/* unmess up Windows file descriptors: there is a warning about an
	 * implicit declaration of _fileno when compiling under Windows in C99
	 * mode */
	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
	_setmode(_fileno(stderr), _O_BINARY);
#endif

done:
	version();
	forth_initial_enviroment(&o, core_size, stdin, stdout, verbose, orig_argc, orig_argv);

	if (readterm) { /* if '-t' or no files given, read from stdin */
		if (verbose >= FORTH_DEBUG_NOTE)
			note("reading from stdin (%p)", stdin);

#ifdef USE_LINE_EDITOR
		if (use_line_editor) {
			rval = forth_line_editor(o, 1);
			goto end;
		}
#endif

		forth_set_file_input(o, stdin);
		rval = forth_run(o);
	}

end:
	fclose_input(&in);

/**
If the save option has been given we only want to save valid core files,
we might want to make an option to force saving of core files for debugging
purposes, but in general we do not want to over write valid previously saved
state with invalid data.
**/

	if (save) { /* save core file */
		if (rval || forth_is_invalid(o)) {
			fatal("refusing to save invalid core, %u/%d", rval, forth_is_invalid(o));
			return -1;
		}
		if (verbose >= FORTH_DEBUG_NOTE)
			note("saving for file to '%s'", dump_name);
		if (forth_save_core_file(o, dump = forth_fopen_or_die(dump_name, "wb"))) {
			fatal("core file save to '%s' failed", dump_name);
			rval = -1;
		}
		fclose(dump);
	}

/**
Whilst the following **forth_free** is not strictly necessary, there
is often a debate that comes up making short lived programs or programs whose
memory use stays either constant or only goes up, when these programs exit
it is not necessary to clean up the environment and in some case (although
not this one) it can slow down the exit of the program for
no reason.  However not freeing the memory after use does not play nice with
programs that detect memory leaks, like Valgrind. Either way, we free the
memory used here, but only if no other errors have occurred before hand.
**/

	forth_free(o);
	return rval;
}

