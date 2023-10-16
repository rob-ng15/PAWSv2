
/*------------------------------------------------------------------------------
    pacman.c

    A Pacman clone written in C99 using the sokol headers for platform
    abstraction.

    The git repository is here:

    https://github.com/floooh/pacman.c

    A WASM version running in browsers can be found here:

    https://floooh.github.io/pacman.c/pacman.html

    Some basic concepts and ideas are worth explaining upfront:

    The game code structure is a bit "radical" and sometimes goes against
    what is considered good practice for medium and large code bases. This is
    fine because this is a small game written by a single person. Small
    code bases written by small teams allow a different organizational
    approach than large code bases written by large teams.

    Here are some of those "extremist" methods used in this tiny project:

    Instead of artificially splitting the code into many small source files,
    everything is in a single source file readable from top to bottom.

    Instead of data-encapsulation and -hiding, all data lives in a single,
    global, nested data structure (this isn't actually as radical and
    experimental as it sounds, I've been using this approach for quite a
    while now for all my hobby code). An interesting side effect of this
    upfront-defined static memory layout is that there are no dynamic
    allocations in the entire game code (only a handful allocations during
    initialization of the Sokol headers).

    Instead of "wasting" time thinking too much about high-level abstractions
    and reusability, the code has been written in a fairly adhoc-manner "from
    start to finish", solving problems as they showed up in the most direct
    way possible. When parts of the code became too entangled I tried to step
    back a bit, take a pause and come back later with a better idea how to
    rewrite those parts in a more straightforward manner. Of course
    "straightforward" and "readability" are in the eye of the beholder.

    The actual gameplay code (Pacman and ghost behaviours) has been
    implemented after the "Pacman Dossier" by Jamey Pittman (a PDF copy has
    been included in the project), but there are some minor differences to a
    Pacman arcade machine emulator, some intended, some not
    (https://floooh.github.io/tiny8bit/pacman.html):

        - The attract mode animation in the intro screen is missing (where
          Pacman is chased by ghosts, eats a pill and then hunts the ghost).
        - Likewise, the 'interlude' animation between levels is missing.
        - Various difficulty-related differences in later maps are ignored
          (such a faster movement speed, smaller dot-counter-limits for ghosts etc)

    The rendering and audio code resembles the original Pacman arcade machine
    hardware:

        - the tile and sprite pixel data, hardware color palette data and
          sound wavetable data is taken directly from embedded arcade machine
          ROM dumps
        - background tiles are rendered from two 28x36 byte buffers (one for
          tile-codes, the other for color-codes), this is similar to an actual
          arcade machine, with the only difference that the tile- and color-buffer
          has a straightforward linear memory layout
        - background tile rendering is done with dynamically uploaded vertex
          data (two triangles per tile), with color-palette decoding done in
          the pixel shader
        - up to 8 16x16 sprites are rendered as vertex quads, with the same
          color palette decoding happening in the pixel shader as for background
          tiles.
        - audio output works through an actual Namco WSG emulator which generates
          sound samples for 3 hardware voices from a 20-bit frequency counter,
          4-bit volume and 3-bit wave type (for 8 wavetables made of 32 sample
          values each stored in a ROM dump)
        - sound effects are implemented by writing new values to the hardware
          voice 'registers' once per 60Hz tick, this can happen in two ways:
            - as 'procedural' sound effects, where a callback function computes
              the new voice register values
            - or via 'register dump' playback, where the voice register values
              have been captured at 60Hz frequency from an actual Pacman arcade
              emulator
          Only two sound effects are register dumps: the little music track at
          the start of a game, and the sound effect when Pacman dies. All other
          effects are simple procedural effects.

    The only concept worth explaining in the gameplay code is how timing
    and 'asynchronous actions' work:

    The entire gameplay logic is driven by a global 60 Hz game tick which is
    counting upward.

    Gameplay actions are initiated by a combination of 'time triggers' and a simple
    vocabulary to initialize and test trigger conditions. This time trigger system
    is an extremely simple replacement for more powerful event systems in
    'proper' game engines.

    Here are some pseudo-code examples how time triggers can be used (unrelated
    to Pacman):

    To immediately trigger an action in one place of the code, and 'realize'
    this action in one or several other places:

        // if a monster has been eaten, trigger the 'monster eaten' action:
        if (monster_eaten()) {
            start(&state.game.monster_eaten);
        }

        // ...somewhere else, we might increase the score if a monster has been eaten:
        if (now(state.game.monster_eaten)) {
            state.game.score += 10;
        }

        // ...and yet somewhere else in the code, we might want to play a sound effect
        if (now(state.game.monster_eaten)) {
            // play sound effect...
        }

    We can also start actions in the future, which allows to batch multiple
    followup-actions in one place:

        // start fading out now, after one second (60 ticks) start a new
        // game round, and fade in, after another second when fadein has
        // finished, start the actual game loop
        start(&state.gfx.fadeout);
        start_after(&state.game.started, 60);
        start_after(&state.gfx.fadein, 60);
        start_after(&state.game.gameloop_started, 2*60);

    As mentioned above, there's a whole little function vocabulary built around
    time triggers, but those are hopefully all self-explanatory.
*/
#include <assert.h>
#include <string.h> // memset()
#include <stdlib.h> // abs()

#include <stdio.h>
#include <PAWSintrinsics.h>
#include <PAWSlibrary.h>

#define int8_t char
#define int16_t short
#define int32_t int
#define int64_t long
#define uint8_t unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned int
#define uint64_t unsigned long

// config defines and global constants
#define AUDIO_VOLUME (0.5f)
#define DBG_SKIP_INTRO      (0)     // set to (1) to skip intro
#define DBG_SKIP_PRELUDE    (0)     // set to (1) to skip game prelude
#define DBG_START_ROUND     (0)     // set to any starting round <=255
#define DBG_MARKERS         (0)     // set to (1) to show debug markers
#define DBG_ESCAPE          (0)     // set to (1) to leave game loop with Esc
#define DBG_DOUBLE_SPEED    (0)     // set to (1) to speed up game (useful with godmode)
#define DBG_GODMODE         (0)     // set to (1) to disable dying

enum {
    NUM_VOICES          = 3,            // number of sound voices
    NUM_SOUNDS          = 3,            // max number of sounds effects that can be active at a time
    NUM_SAMPLES         = 128,          // max number of audio samples in local sample buffer
    DISABLED_TICKS      = 0xFFFFFFFF,   // magic tick value for a disabled timer
    TILE_WIDTH          = 8,            // width and height of a background tile in pixels
    TILE_HEIGHT         = 8,
    SPRITE_WIDTH        = 16,           // width and height of a sprite in pixels
    SPRITE_HEIGHT       = 16,
    DISPLAY_TILES_X     = 28,           // tile buffer width and height
    DISPLAY_TILES_Y     = 36,
    DISPLAY_PIXELS_X    = DISPLAY_TILES_X * TILE_WIDTH,
    DISPLAY_PIXELS_Y    = DISPLAY_TILES_Y * TILE_HEIGHT,
    NUM_SPRITES         = 8,
    NUM_DEBUG_MARKERS   = 16,
    TILE_TEXTURE_WIDTH  = 256 * TILE_WIDTH,
    TILE_TEXTURE_HEIGHT = TILE_HEIGHT + SPRITE_HEIGHT,
    MAX_VERTICES        = ((DISPLAY_TILES_X * DISPLAY_TILES_Y) + NUM_SPRITES + NUM_DEBUG_MARKERS) * 6,
    FADE_TICKS          = 30,   // duration of fade-in/out
    NUM_LIVES           = 3,
    NUM_STATUS_FRUITS   = 7,    // max number of displayed fruits at bottom right
    NUM_DOTS            = 244,  // 240 small dots + 4 pills
    NUM_PILLS           = 4,    // number of energizer pills on playfield
    ANTEPORTAS_X        = 14*TILE_WIDTH,  // pixel position of the ghost house enter/leave point
    ANTEPORTAS_Y        = 14*TILE_HEIGHT + TILE_HEIGHT/2,
    GHOST_EATEN_FREEZE_TICKS = 60,  // number of ticks the game freezes after Pacman eats a ghost
    PACMAN_EATEN_TICKS  = 60,       // number of ticks to freeze game when Pacman is eaten
    PACMAN_DEATH_TICKS  = 150,      // number of ticks to show the Pacman death sequence before starting new round
    GAMEOVER_TICKS      = 3*60,     // number of ticks the game over message is shown
    ROUNDWON_TICKS      = 4*60,     // number of ticks to wait after a round was won
    FRUITACTIVE_TICKS   = 10*60,    // number of ticks a bonus fruit is shown
};

/* common tile, sprite and color codes, these are the same as on the Pacman
   arcade machine and extracted by looking at memory locations of a Pacman emulator
*/
enum {
    TILE_SPACE          = 0,
    TILE_DOT            = 12,
    TILE_PILL           = 14,
    TILE_GHOST          = 56,
    TILE_LIFE           = 0x20, // 0x20..0x23
    TILE_CHERRIES       = 48, // 0x90..0x93
    TILE_STRAWBERRY     = 49, // 0x94..0x97
    TILE_PEACH          = 50, // 0x98..0x9B
    TILE_BELL           = 51, // 0x9C..0x9F
    TILE_APPLE          = 52, // 0xA0..0xA3
    TILE_GRAPES         = 53, // 0xA4..0xA7
    TILE_GALAXIAN       = 54, // 0xA8..0xAB
    TILE_KEY            = 55, // 0xAC..0xAF
    TILE_DOOR           = 0x09, // the ghost-house door

    SPRITETILE_INVISIBLE    = 30,
    SPRITETILE_SCORE_200    = 0,
    SPRITETILE_SCORE_400    = 1,
    SPRITETILE_SCORE_800    = 2,
    SPRITETILE_SCORE_1600   = 3,
    SPRITETILE_CHERRIES     = 0,
    SPRITETILE_STRAWBERRY   = 1,
    SPRITETILE_PEACH        = 2,
    SPRITETILE_BELL         = 3,
    SPRITETILE_APPLE        = 4,
    SPRITETILE_GRAPES       = 5,
    SPRITETILE_GALAXIAN     = 6,
    SPRITETILE_KEY          = 7,
    SPRITETILE_PACMAN_CLOSED_MOUTH = 48,

    COLOR_BLANK         = 0x00,
    COLOR_DEFAULT       = 255,
    COLOR_DOT           = 237,
    COLOR_PACMAN        = 249,
    COLOR_BLINKY        = 201,
    COLOR_PINKY         = 239,
    COLOR_INKY          = 63,
    COLOR_CLYDE         = 233,
    COLOR_FRIGHTENED    = 15,
    COLOR_FRIGHTENED_BLINKING = 247,
    COLOR_GHOST_SCORE   = 0x18,
    COLOR_EYES          = 0x19,
    COLOR_CHERRIES      = 0x14,
    COLOR_STRAWBERRY    = 0x0F,
    COLOR_PEACH         = 0x15,
    COLOR_BELL          = 0x16,
    COLOR_APPLE         = 0x14,
    COLOR_GRAPES        = 0x17,
    COLOR_GALAXIAN      = 0x09,
    COLOR_KEY           = 0x16,
    COLOR_WHITE_BORDER  = 0x1F,
    COLOR_FRUIT_SCORE   = 0x03,
};

// the top-level game states (intro => game => intro)
typedef enum {
    GAMESTATE_INTRO,
    GAMESTATE_GAME,
} gamestate_t;

// directions NOTE: bit0==0: horizontal movement, bit0==1: vertical movement
typedef enum {
    DIR_RIGHT,  // 000
    DIR_DOWN,   // 001
    DIR_LEFT,   // 010
    DIR_UP,     // 011
    NUM_DIRS
} dir_t;

// bonus fruit types
typedef enum {
    FRUIT_NONE,
    FRUIT_CHERRIES,
    FRUIT_STRAWBERRY,
    FRUIT_PEACH,
    FRUIT_APPLE,
    FRUIT_GRAPES,
    FRUIT_GALAXIAN,
    FRUIT_BELL,
    FRUIT_KEY,
    NUM_FRUITS
} fruit_t;

// sprite 'hardware' indices
typedef enum {
    SPRITE_PACMAN,
    SPRITE_BLINKY,
    SPRITE_PINKY,
    SPRITE_INKY,
    SPRITE_CLYDE,
    SPRITE_FRUIT,
} sprite_index_t;

// ghost types
typedef enum {
    GHOSTTYPE_BLINKY,
    GHOSTTYPE_PINKY,
    GHOSTTYPE_INKY,
    GHOSTTYPE_CLYDE,
    NUM_GHOSTS
} ghosttype_t;

// ghost AI states
typedef enum {
    GHOSTSTATE_NONE,
    GHOSTSTATE_CHASE,           // currently chasing Pacman
    GHOSTSTATE_SCATTER,         // currently heading to the corner scatter targets
    GHOSTSTATE_FRIGHTENED,      // frightened after Pacman has eaten an energizer pill
    GHOSTSTATE_EYES,            // eaten by Pacman and heading back to the ghost house
    GHOSTSTATE_HOUSE,           // currently inside the ghost house
    GHOSTSTATE_LEAVEHOUSE,      // currently leaving the ghost house
    GHOSTSTATE_ENTERHOUSE       // currently entering the ghost house
} ghoststate_t;

// reasons why game loop is frozen
typedef enum {
    FREEZETYPE_PRELUDE   = (1<<0),  // game prelude is active (with the game start tune playing)
    FREEZETYPE_READY     = (1<<1),  // READY! phase is active (at start of a new game round)
    FREEZETYPE_EAT_GHOST = (1<<2),  // Pacman has eaten a ghost
    FREEZETYPE_DEAD      = (1<<3),  // Pacman was eaten by a ghost
    FREEZETYPE_WON       = (1<<4),  // game round was won by eating all dots
} freezetype_t;

// a trigger holds a specific game-tick when an action should be started
typedef struct {
    uint32_t tick;
} trigger_t;

// a 2D integer vector (used both for pixel- and tile-coordinates)
typedef struct {
    int16_t x;
    int16_t y;
} int2_t;

// common state for pacman and ghosts
typedef struct {
    dir_t dir;              // current movement direction
    int2_t pos;             // position of sprite center in pixel coords
    uint32_t anim_tick;     // incremented when actor moved in current tick
} actor_t;

// ghost AI state
typedef struct {
    actor_t actor;
    ghosttype_t type;
    dir_t next_dir;         // ghost AI looks ahead one tile when deciding movement direction
    int2_t target_pos;      // current target position in tile coordinates
    ghoststate_t state;
    trigger_t frightened;   // game tick when frightened mode was entered
    trigger_t eaten;        // game tick when eaten by Pacman
    uint16_t dot_counter;   // used to decide when to leave the ghost house
    uint16_t dot_limit;
} ghost_t;

// pacman state
typedef struct {
    actor_t actor;
} pacman_t;

// the tile- and sprite-renderer's vertex structure
typedef struct {
    float x, y;         // screen coords [0..1]
    float u, v;         // tile texture coords
    uint32_t attr;      // x: color code, y: opacity (opacity only used for fade effect)
} vertex_t;

// sprite state
typedef struct {
    bool enabled;           // if false sprite is deactivated
    uint8_t tile, color;    // sprite-tile number (0..63), color code
    bool flipx, flipy;      // horizontal/vertical flip
    int2_t pos;             // pixel position of the sprite's top-left corner
} sprite_t;

// debug visualization markers (see DBG_MARKERS)
typedef struct {
    bool enabled;
    uint8_t tile, color;    // tile and color code
    int2_t tile_pos;
} debugmarker_t;

// all state is in a single nested struct
static struct {

    gamestate_t gamestate;  // the current gamestate (intro => game => intro)

    struct {
        uint32_t tick;          // the central game tick, this drives the whole game
        uint64_t laptime_store; // helper variable to measure frame duration
        int32_t tick_accum;     // helper variable to decouple ticks from frame rate
    } timing;

    // intro state
    struct {
        trigger_t started;      // tick when intro-state was started
    } intro;

    // game state
    struct {
        uint32_t xorshift;          // current xorshift random-number-generator state
        uint32_t hiscore;           // hiscore / 10
        trigger_t started;
        trigger_t ready_started;
        trigger_t round_started;
        trigger_t round_won;
        trigger_t game_over;
        trigger_t dot_eaten;            // last time Pacman ate a dot
        trigger_t pill_eaten;           // last time Pacman ate a pill
        trigger_t ghost_eaten;          // last time Pacman ate a ghost
        trigger_t pacman_eaten;         // last time Pacman was eaten by a ghost
        trigger_t fruit_eaten;          // last time Pacman has eaten the bonus fruit
        trigger_t force_leave_house;    // starts when a dot is eaten
        trigger_t fruit_active;         // starts when bonus fruit is shown
        uint8_t freeze;                 // combination of FREEZETYPE_* flags
        uint8_t round;                  // current game round, 0, 1, 2...
        uint32_t score;                 // score / 10
        int8_t num_lives;
        uint8_t num_ghosts_eaten;       // number of ghosts easten with current pill
        uint8_t num_dots_eaten;         // if == NUM_DOTS, Pacman wins the round
        bool global_dot_counter_active;     // set to true when Pacman loses a life
        uint8_t global_dot_counter;         // the global dot counter for the ghost-house-logic
        ghost_t ghost[NUM_GHOSTS];
        pacman_t pacman;
        fruit_t active_fruit;
    } game;

    // the current input state
    struct {
        bool enabled;
        bool up;
        bool down;
        bool left;
        bool right;
        bool esc;       // only for debugging (see DBG_ESCACPE)
        bool anykey;
    } input;

    // the gfx subsystem implements a simple tile+sprite renderer
    struct {
        // fade-in/out timers and current value
        trigger_t fadein;
        trigger_t fadeout;
        uint8_t fade;

        // the 36x28 tile framebuffer
        uint8_t video_ram[DISPLAY_TILES_Y][DISPLAY_TILES_X]; // tile codes

        // up to 8 sprites
        sprite_t sprite[NUM_SPRITES];

        // up to 16 debug markers
        debugmarker_t debug_marker[NUM_DEBUG_MARKERS];

        // intermediate vertex buffer for tile- and sprite-rendering
        int num_vertices;
        vertex_t vertices[MAX_VERTICES];

        // scratch-buffer for tile-decoding (only happens once)
        uint8_t tile_pixels[TILE_TEXTURE_HEIGHT][TILE_TEXTURE_WIDTH];

        // scratch buffer for the color palette
        uint32_t color_palette[256];
    } gfx;
} state;

// scatter target positions (in tile coords)
static const int2_t ghost_scatter_targets[NUM_GHOSTS] = {
    { 25, 0 }, { 2, 0 }, { 27, 34 }, { 0, 34 }
};

// starting positions for ghosts (pixel coords)
static const int2_t ghost_starting_pos[NUM_GHOSTS] = {
    { 14*8, 14*8 + 4 },
    { 14*8, 17*8 + 4 },
    { 12*8, 17*8 + 4 },
    { 16*8, 17*8 + 4 },
};

// target positions for ghost entering the ghost house (pixel coords)
static const int2_t ghost_house_target_pos[NUM_GHOSTS] = {
    { 14*8, 17*8 + 4 },
    { 14*8, 17*8 + 4 },
    { 12*8, 17*8 + 4 },
    { 16*8, 17*8 + 4 },
};

// fruit tiles, sprite tiles and colors
static const uint8_t fruit_tiles_colors[NUM_FRUITS][3] = {
    { 0, 0, 0 },   // FRUIT_NONE
    { TILE_CHERRIES,    SPRITETILE_CHERRIES,    COLOR_CHERRIES },
    { TILE_STRAWBERRY,  SPRITETILE_STRAWBERRY,  COLOR_STRAWBERRY },
    { TILE_PEACH,       SPRITETILE_PEACH,       COLOR_PEACH },
    { TILE_APPLE,       SPRITETILE_APPLE,       COLOR_APPLE },
    { TILE_GRAPES,      SPRITETILE_GRAPES,      COLOR_GRAPES },
    { TILE_GALAXIAN,    SPRITETILE_GALAXIAN,    COLOR_GALAXIAN },
    { TILE_BELL,        SPRITETILE_BELL,        COLOR_BELL },
    { TILE_KEY,         SPRITETILE_KEY,         COLOR_KEY }
};

// the tiles for displaying the bonus-fruit-score, this is a number built from 4 tiles
static const uint8_t fruit_score_tiles[NUM_FRUITS][4] = {
    { 0x40, 0x40, 0x40, 0x40 }, // FRUIT_NONE
    { 0x40, 0x81, 0x85, 0x40 }, // FRUIT_CHERRIES: 100
    { 0x40, 0x82, 0x85, 0x40 }, // FRUIT_STRAWBERRY: 300
    { 0x40, 0x83, 0x85, 0x40 }, // FRUIT_PEACH: 500
    { 0x40, 0x84, 0x85, 0x40 }, // FRUIT_APPLE: 700
    { 0x40, 0x86, 0x8D, 0x8E }, // FRUIT_GRAPES: 1000
    { 0x87, 0x88, 0x8D, 0x8E }, // FRUIT_GALAXIAN: 2000
    { 0x89, 0x8A, 0x8D, 0x8E }, // FRUIT_BELL: 3000
    { 0x8B, 0x8C, 0x8D, 0x8E }, // FRUIT_KEY: 5000
};

// level specifications (see pacman_dossier.pdf)
typedef struct {
    fruit_t bonus_fruit;
    uint16_t bonus_score;
    uint16_t fright_ticks;
    // FIXME: the various Pacman and ghost speeds
} levelspec_t;

enum {
    MAX_LEVELSPEC = 21,
};
static const levelspec_t levelspec_table[MAX_LEVELSPEC] = {
    { FRUIT_CHERRIES,   10,  6*60, },
    { FRUIT_STRAWBERRY, 30,  5*60, },
    { FRUIT_PEACH,      50,  4*60, },
    { FRUIT_PEACH,      50,  3*60, },
    { FRUIT_APPLE,      70,  2*60, },
    { FRUIT_APPLE,      70,  5*60, },
    { FRUIT_GRAPES,     100, 2*60, },
    { FRUIT_GRAPES,     100, 2*60, },
    { FRUIT_GALAXIAN,   200, 1*60, },
    { FRUIT_GALAXIAN,   200, 5*60, },
    { FRUIT_BELL,       300, 2*60, },
    { FRUIT_BELL,       300, 1*60, },
    { FRUIT_KEY,        500, 1*60, },
    { FRUIT_KEY,        500, 3*60, },
    { FRUIT_KEY,        500, 1*60, },
    { FRUIT_KEY,        500, 1*60, },
    { FRUIT_KEY,        500, 1,    },
    { FRUIT_KEY,        500, 1*60, },
    { FRUIT_KEY,        500, 1,    },
    { FRUIT_KEY,        500, 1,    },
    { FRUIT_KEY,        500, 1,    },
    // from here on repeating
};

// PAWS sound effects
unsigned char tune_treble[] = { 51, 51, 75, 75, 65, 65, 59, 59, 75, 63, 51, 51, 57, 57, 0, 0,
                                75, 75, 77, 77, 65, 65, 61, 61, 77, 65, 53, 53, 61, 61, 0, 0,
                                51, 51, 75, 75, 65, 65, 59, 59, 75, 63, 51, 51, 57, 57, 0, 0,
                                59, 61, 63,  0, 63, 65, 65,  0, 65, 69, 71,  0, 75, 75, 75, 0 };
unsigned char tune_bass[] = { 27,  0,  0, 41, 27,  0,  0, 43,
                              29,  0,  0, 43, 29,  0,  0, 41,
                              27,  0,  0, 41, 27,  0,  0, 43,
                              41,  0, 43,  0, 47,  0,  51, 0 };

unsigned char eat_dot_1[6] = { 107, 111, 115, 119, 123, 127 };
unsigned char eat_dot_2[6] = { 69, 82, 91, 98, 103, 106 };
unsigned char eat_fruit[] = { 109, 106, 103, 100, 97, 94, 91, 88, 85, 82, 79, 82, 85, 88, 91, 94, 97, 100, 103, 106, 109, 112, 115, 118 };
unsigned char eat_ghost[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 0 };
unsigned char eat_pacman[90] = { 121, 120, 119, 118, 117, 118, 119, 120, 121, 122, 123, 119, 118, 117, 116, 115, 114, 115, 116, 117, 118, 119, 120, 117,
                                 116, 115, 114, 113, 112, 113, 114, 115, 116, 117, 118, 115, 114, 113, 112, 111, 110, 111, 112, 113, 114, 115, 116, 113,
                                 112, 111, 110, 109, 108, 109, 110, 111, 112, 113, 114, 111, 110, 109, 108, 107, 106, 107, 108, 74, 76, 78, 80, 82, 84,
                                 86, 88, 90, 92, 94, 0, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94 };
unsigned char alert_normal[] = { 98, 102, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 124, 122, 120, 118, 116, 114, 112, 110, 108 };
unsigned char alert_frightended[] = { 28, 42, 54, 64, 72, 78, 80, 82 };

#define SND_START_INTRO 1
#define SND_START_DOT1 2
#define SND_START_DOT2 3
#define SND_START_PILL 4
#define SND_START_FRUIT 5
#define SND_START_GHOST 6
#define SND_START_PACMAN 7
#define SND_START_NORMAL 8
#define SND_START_FRIGHTENDED 9
#define SND_STOP_ALL 255

// PAWS BITMAPS
unsigned char tilemap_lower[] = {
    #include "graphics/tilemap_lower.h"
};
unsigned char tilemap_lower_alt[] = {
    #include "graphics/tilemap_lower-alt.h"
};
unsigned char tilemap_upper[] = {
    #include "graphics/tilemap_upper.h"
};
unsigned char sprite_upper[] = {
    #include "graphics/sprite_upper.h"
};

// INCLUDE 3D PACMAN BACKDROP
#include "graphics/3DPACMAN_BMP.h"

// CONVERT FROM PACMAN COORDINATES TO PAWS_TILE COORDINATES
#define tocoords(a,b) a*2+96,b*2-57

// forward declarations
static void init(void);
static void frame(void);
static void cleanup(void);
static void input();

static void start(trigger_t* t);
static bool now(trigger_t t);

static void pacman_tick(void);
static void intro_tick(void);
static void game_tick(void);

static void input_enable(void);
static void input_disable(void);

static void gfx_init(void);
static void gfx_shutdown(void);
static void gfx_fade(void);
static void gfx_draw(void);

static void paws_snd( int action );
static void snd_shutdown(void);

static void init(void) {
    gfx_init();

    // start into intro screen
    #if DBG_SKIP_INTRO
        start(&state.game.started);
    #else
        start(&state.intro.started);
    #endif
}

// RUN ONE FRAME, SPEED IS CONTROLLED BY THE MAIN LOOP, WHICH LIMITS TO 60 fps to match original speed
static void frame(void) {
    state.timing.tick++;

    // check for game state change
    if (now(state.intro.started)) {
        state.gamestate = GAMESTATE_INTRO;
    }
    if (now(state.game.started)) {
        state.gamestate = GAMESTATE_GAME;
    }

    // call the top-level game state update function
    switch (state.gamestate) {
        case GAMESTATE_INTRO:
            intro_tick();
            break;
        case GAMESTATE_GAME:
            game_tick();
            break;
    }
    gfx_draw();
}

static void input() {
    if (state.input.enabled) {
        unsigned short joystick = get_buttons() & 0xfffe;
        state.input.anykey = ( joystick != 0 );
        state.input.up = ( joystick & 8 ) >> 3;
        state.input.down = ( joystick & 16 ) >> 4;
        state.input.right = ( joystick & 64 ) >> 6;
        state.input.left = ( joystick & 32 ) >> 5;
    } else {
        state.input.anykey = state.input.up = state.input.down = state.input.right = state.input.left = 0;
    }
}

static void cleanup(void) {
    snd_shutdown();
    gfx_shutdown();
}

/*== GRAB BAG OF HELPER FUNCTIONS ============================================*/

// xorshift random number generator
static uint32_t xorshift32(void) {
    uint32_t x = state.game.xorshift;
    x ^= x<<13;
    x ^= x>>17;
    x ^= x<<5;
    return state.game.xorshift = x;
}
// get level spec for a game round
static levelspec_t levelspec(int round) {
    if (round >= MAX_LEVELSPEC) {
        round = MAX_LEVELSPEC-1;
    }
    return levelspec_table[round];
}

// set time trigger to the next game tick
static void start(trigger_t* t) {
    t->tick = state.timing.tick + 1;
}

// set time trigger to a future tick
static void start_after(trigger_t* t, uint32_t ticks) {
    t->tick = state.timing.tick + ticks;
}

// deactivate a time trigger
static void disable(trigger_t* t) {
    t->tick = DISABLED_TICKS;
}

// return a disabled time trigger
static trigger_t disabled_timer(void) {
    return (trigger_t) { .tick = DISABLED_TICKS };
}

// check if a time trigger is triggered
static bool now(trigger_t t) {
    return t.tick == state.timing.tick;
}

// return the number of ticks since a time trigger was triggered
static uint32_t since(trigger_t t) {
    if (state.timing.tick >= t.tick) {
        return state.timing.tick - t.tick;
    }
    else {
        return DISABLED_TICKS;
    }
}

// check if a time trigger is between begin and end tick
static bool between(trigger_t t, uint32_t begin, uint32_t end) {
    if (t.tick != DISABLED_TICKS) {
        uint32_t ticks = since(t);
        return (ticks >= begin) && (ticks < end);
    }
    else {
        return false;
    }
}

// check if a time trigger was triggered exactly N ticks ago
static bool after_once(trigger_t t, uint32_t ticks) {
    return since(t) == ticks;
}

// check if a time trigger was triggered more than N ticks ago
static bool after(trigger_t t, uint32_t ticks) {
    uint32_t s = since(t);
    if (s != DISABLED_TICKS) {
        return s >= ticks;
    }
    else {
        return false;
    }
}

// same as between(t, 0, ticks)
static bool before(trigger_t t, uint32_t ticks) {
    uint32_t s = since(t);
    if (s != DISABLED_TICKS) {
        return s < ticks;
    }
    else {
        return false;
    }
}

// clear input state and disable input
static void input_disable(void) {
    memset(&state.input, 0, sizeof(state.input));
}

// enable input again
static void input_enable(void) {
    state.input.enabled = true;
}

// get the current input as dir_t
static dir_t input_dir(dir_t default_dir) {
    if (state.input.up) {
        return DIR_UP;
    }
    else if (state.input.down) {
        return DIR_DOWN;
    }
    else if (state.input.right) {
        return DIR_RIGHT;
    }
    else if (state.input.left) {
        return DIR_LEFT;
    }
    else {
        return default_dir;
    }
}

// shortcut to create an int2_t
static int2_t i2(int16_t x, int16_t y) {
    return (int2_t) { x, y };
}

// add two int2_t
static int2_t add_i2(int2_t v0, int2_t v1) {
    return (int2_t) { v0.x+v1.x, v0.y+v1.y };
}

// subtract two int2_t
static int2_t sub_i2(int2_t v0, int2_t v1) {
    return (int2_t) { v0.x-v1.x, v0.y-v1.y };
}

// multiply int2_t with scalar
static int2_t mul_i2(int2_t v, int16_t s) {
    return (int2_t) { v.x*s, v.y*s };
}

// squared-distance between two int2_t
static int32_t squared_distance_i2(int2_t v0, int2_t v1) {
    int2_t d = { v1.x - v0.x, v1.y - v0.y };
    return d.x * d.x + d.y * d.y;
}

// check if two int2_t are equal
static bool equal_i2(int2_t v0, int2_t v1) {
    return (v0.x == v1.x) && (v0.y == v1.y);
}

// check if two int2_t are nearly equal
static bool nearequal_i2(int2_t v0, int2_t v1, int16_t tolerance) {
    return (abs(v1.x - v0.x) <= tolerance) && (abs(v1.y - v0.y) <= tolerance);
}

// convert an actor pos (origin at center) to sprite pos (origin top left)
static int2_t actor_to_sprite_pos(int2_t pos) {
    return i2(pos.x - SPRITE_WIDTH/2, pos.y - SPRITE_HEIGHT/2);
}

// compute the distance of a pixel coordinate to the next tile midpoint
int2_t dist_to_tile_mid(int2_t pos) {
    return i2((TILE_WIDTH/2) - pos.x % TILE_WIDTH, (TILE_HEIGHT/2) - pos.y % TILE_HEIGHT);
}

// clear tile buffer
static void vid_clear(uint8_t tile_code, uint8_t color_code) {
    memset(&state.gfx.video_ram, tile_code, sizeof(state.gfx.video_ram));
    tilemap_scrollwrapclear( LOWER_LAYER, TM_CLEAR );
    tilemap_scrollwrapclear( UPPER_LAYER, TM_CLEAR );
    gpu_cs();

    // CLEAR CENTRE TEXT
    for( int y = 0; y < 30; y++ ) {
        tpu_set( 6, y, TRANSPARENT, WHITE );
        tpu_outputstring( 0, "                            " );
    }
    // CLEAR SCORE / HISCORE
    if(state.game.num_lives) {
        tpu_set( 0, 1, TRANSPARENT, WHITE ); tpu_outputstring( 0, "      " );
    }
    tpu_set( 0, 28, TRANSPARENT, WHITE ); tpu_outputstring( 0, "      " );
}

// check if a tile position is valid
static bool valid_tile_pos(int2_t tile_pos) {
    return ((tile_pos.x >= 0) && (tile_pos.x < DISPLAY_TILES_X) && (tile_pos.y >= 0) && (tile_pos.y < DISPLAY_TILES_Y));
}

static void paws_tile(int2_t tile_pos, uint8_t tile_code) {
    set_tilemap_tile( LOWER_LAYER, tile_pos.x + 8, tile_pos.y - 1, tile_code, 0 );
    state.gfx.video_ram[tile_pos.y][tile_pos.x] = tile_code;
}

// translate ASCII char into "NAMCO char"
static char conv_char(char c) {
    switch (c) {
        case ' ':   c = 0x40; break;
        case '/':   c = 58; break;
        case '-':   c = 59; break;
        case '\"':  c = 38; break;
        case '!':   c = 'Z'+1; break;
        default: break;
    }
    return c;
}

// put colored text into tpu buffer
static void vid_color_text(int2_t tile_pos, uint8_t color_code, const char* text) {
    tpu_set( tile_pos.x, tile_pos.y, TRANSPARENT, color_code );
    tpu_outputstring( BOLD, (char *)text );
}

// put text into tpu buffer
static void vid_text(int2_t tile_pos, const char* text) {
    tpu_set( tile_pos.x, tile_pos.y, TRANSPARENT, COLOR_DEFAULT );
    tpu_outputstring( BOLD, (char *)text );
}

/* print colored score number into tile+color buffers from right to left(!),
    scores are /10, the last printed number is always 0,
    a zero-score will print as '00' (this is the same as on
    the Pacman arcade machine)
*/
static void vid_color_score(int2_t tile_pos, uint8_t color_code, uint32_t score) {
    tpu_set( tile_pos.x, tile_pos.y, TRANSPARENT, color_code );
    tpu_output_character( '0' );
    tile_pos.x--;
    for (int digit = 0; digit < 8; digit++) {
        char chr = (score % 10) + '0';
        tpu_set( tile_pos.x, tile_pos.y, TRANSPARENT, color_code );
        tpu_output_character( chr );
        tile_pos.x--;
        score /= 10;
        if (0 == score) {
            break;
        }
    }
}

// draw the fruit bonus score tiles (when Pacman has eaten the bonus fruit)
static void vid_fruit_score(fruit_t fruit_type) {
    if(fruit_type != FRUIT_NONE) {
        set_sprite( UPPER_LAYER, 13, TRUE, 304, 256, fruit_type-1, SPRITE_DOUBLE );
    } else {
        set_sprite_attribute( UPPER_LAYER, 13, SPRITE_ACTIVE, FALSE );
    }
}

// disable and clear all sprites
static void spr_clear(void) {
    memset(&state.gfx.sprite, 0, sizeof(state.gfx.sprite));
    for( int i = 0; i < 16; i++ )
        set_sprite_attribute( UPPER_LAYER, i, SPRITE_ACTIVE, FALSE );
}

// get pointer to pacman sprite
static sprite_t* spr_pacman(void) {
    return &state.gfx.sprite[SPRITE_PACMAN];
}

// get pointer to ghost sprite
static sprite_t* spr_ghost(ghosttype_t type) {
    return &state.gfx.sprite[SPRITE_BLINKY + type];
}

// get pointer to fruit sprite
static sprite_t* spr_fruit(void) {
    return &state.gfx.sprite[SPRITE_FRUIT];
}

// set sprite to animated Pacman
static void spr_anim_pacman(dir_t dir, uint32_t tick) {
    // animation frames for horizontal and vertical movement
    static const uint8_t tiles[2][4] = {
        { 44, 46, 48, 46 }, // horizontal (needs flipx)
        { 45, 47, 48, 47 }  // vertical (needs flipy)
    };
    sprite_t* spr = spr_pacman();
    uint32_t phase = (tick / 4) & 3;
    spr->tile  = tiles[dir & 1][phase];
    spr->color = COLOR_PACMAN;
    spr->flipx = (dir == DIR_LEFT);
    spr->flipy = (dir == DIR_UP);
}

// set sprite to Pacman's death sequence
static void spr_anim_pacman_death(uint32_t tick) {
    // the death animation tile sequence starts at sprite tile number 52 and ends at 63
    sprite_t* spr = spr_pacman();
    uint32_t tile = 52 + (tick / 8);
    if (tile > 63) {
        tile = 63;
    }
    spr->tile = tile;
    spr->flipx = spr->flipy = false;
}

// set sprite to animated ghost
static void spr_anim_ghost(ghosttype_t ghost_type, dir_t dir, uint32_t tick) {
    static const uint8_t tiles[4][2]  = {
        { 32, 33 }, // right
        { 34, 35 }, // down
        { 36, 37 }, // left
        { 38, 39 }, // up
    };
    uint32_t phase = (tick / 8) & 1;
    sprite_t* spr = spr_ghost(ghost_type);
    spr->tile = tiles[dir][phase];
    spr->color = COLOR_BLINKY + 2*ghost_type;
    spr->flipx = false;
    spr->flipy = false;
}

// set sprite to frightened ghost
static void spr_anim_ghost_frightened(ghosttype_t ghost_type, uint32_t tick) {
    static const uint8_t tiles[2] = { 28, 29 };
    uint32_t phase = (tick / 4) & 1;
    sprite_t* spr = spr_ghost(ghost_type);
    spr->tile = tiles[phase];
    if (tick > (uint32_t)(levelspec(state.game.round).fright_ticks - 60)) {
        // towards end of frightening period, start blinking
        spr->color = (tick & 0x10) ? COLOR_FRIGHTENED : COLOR_FRIGHTENED_BLINKING;
    }
    else {
        spr->color = COLOR_FRIGHTENED;
    }
    spr->flipx = false;
    spr->flipy = false;
}

/* set sprite to ghost eyes, these are the normal ghost sprite
    images but with a different color code which makes
    only the eyes visible
*/
static void spr_anim_ghost_eyes(ghosttype_t ghost_type, dir_t dir) {
    static const uint8_t tiles[NUM_DIRS] = { 32, 34, 36, 38 };
    sprite_t* spr = spr_ghost(ghost_type);
    spr->tile = tiles[dir];
    spr->color = COLOR_EYES;
    spr->flipx = false;
    spr->flipy = false;
}

// convert pixel position to tile position
static int2_t pixel_to_tile_pos(int2_t pix_pos) {
    return i2(pix_pos.x / TILE_WIDTH, pix_pos.y / TILE_HEIGHT);
}

// clamp tile pos to valid playfield coords
static int2_t clamped_tile_pos(int2_t tile_pos) {
    int2_t res = tile_pos;
    if (res.x < 0) {
        res.x = 0;
    }
    else if (res.x >= DISPLAY_TILES_X) {
        res.x = DISPLAY_TILES_X - 1;
    }
    if (res.y < 3) {
        res.y = 3;
    }
    else if (res.y >= (DISPLAY_TILES_Y-2)) {
        res.y = DISPLAY_TILES_Y - 3;
    }
    return res;
}

// convert a direction to a movement vector
static int2_t dir_to_vec(dir_t dir) {
    static const int2_t dir_map[NUM_DIRS] = { { +1, 0 }, { 0, +1 }, { -1, 0 }, { 0, -1 } };
    return dir_map[dir];
}

// return the reverse direction
static dir_t reverse_dir(dir_t dir) {
    switch (dir) {
        case DIR_RIGHT: return DIR_LEFT;
        case DIR_DOWN:  return DIR_UP;
        case DIR_LEFT:  return DIR_RIGHT;
        default:        return DIR_DOWN;
    }
}

// return tile code at tile position
static uint8_t tile_code_at(int2_t tile_pos) {
    return state.gfx.video_ram[tile_pos.y][tile_pos.x];
}

// check if a tile position contains a blocking tile (walls and ghost house door)
static bool is_blocking_tile(int2_t tile_pos) {
    return ( tile_code_at(tile_pos) > 0 ) && ( tile_code_at(tile_pos) < TILE_DOT );
}

// check if a tile position contains a dot tile
static bool is_dot(int2_t tile_pos) {
    return tile_code_at(tile_pos) == TILE_DOT;
}

// check if a tile position contains a pill tile
static bool is_pill(int2_t tile_pos) {
    return ( tile_code_at(tile_pos) == TILE_PILL ) || ( tile_code_at(tile_pos) == TILE_PILL-1 );
}

// check if a tile position is in the teleport tunnel
static bool is_tunnel(int2_t tile_pos) {
    return (tile_pos.y == 17) && ((tile_pos.x <= 5) || (tile_pos.x >= 22));
}

// check if a position is in the ghost's red zone, where upward movement is forbidden
// (see Pacman Dossier "Areas To Exploit")
static bool is_redzone(int2_t tile_pos) {
    return ((tile_pos.x >= 11) && (tile_pos.x <= 16) && ((tile_pos.y == 14) || (tile_pos.y == 26)));
}

// test if movement from a pixel position in a wanted direction is possible,
// allow_cornering is Pacman's feature to take a diagonal shortcut around corners
static bool can_move(int2_t pos, dir_t wanted_dir, bool allow_cornering) {
    const int2_t dir_vec = dir_to_vec(wanted_dir);
    const int2_t dist_mid = dist_to_tile_mid(pos);

    // distance to midpoint in move direction and perpendicular direction
    int16_t move_dist_mid, perp_dist_mid;
    if (dir_vec.y != 0) {
        move_dist_mid = dist_mid.y;
        perp_dist_mid = dist_mid.x;
    }
    else {
        move_dist_mid = dist_mid.x;
        perp_dist_mid = dist_mid.y;
    }

    // look one tile ahead in movement direction
    const int2_t tile_pos = pixel_to_tile_pos(pos);
    const int2_t check_pos = clamped_tile_pos(add_i2(tile_pos, dir_vec));
    const bool is_blocked = is_blocking_tile(check_pos);
    if ((!allow_cornering && (0 != perp_dist_mid)) || (is_blocked && (0 == move_dist_mid))) {
        // way is blocked
        return false;
    }
    else {
        // way is free
        return true;
    }
}

// compute a new pixel position along a direction (without blocking check!)
static int2_t move(int2_t pos, dir_t dir, bool allow_cornering) {
    const int2_t dir_vec = dir_to_vec(dir);
    pos = add_i2(pos, dir_vec);

    // if cornering is allowed, drag the position towards the center-line
    if (allow_cornering) {
        const int2_t dist_mid = dist_to_tile_mid(pos);
        if (dir_vec.x != 0) {
            if (dist_mid.y < 0)      { pos.y--; }
            else if (dist_mid.y > 0) { pos.y++; }
        }
        else if (dir_vec.y != 0) {
            if (dist_mid.x < 0)      { pos.x--; }
            else if (dist_mid.x > 0) { pos.x++; }
        }
    }

    // wrap x-position around (only possible in the teleport-tunnel)
    if (pos.x < 0) {
        pos.x = DISPLAY_PIXELS_X - 1;
    }
    else if (pos.x >= DISPLAY_PIXELS_X) {
        pos.x = 0;
    }
    return pos;
}

// disable and reset all debug markers
static void dbg_clear(void) {
    memset(&state.gfx.debug_marker, 0, sizeof(state.gfx.debug_marker));
}

// set a debug marker
static void dbg_marker(int index, int2_t tile_pos, uint8_t tile_code, uint8_t color_code) {
    state.gfx.debug_marker[index] = (debugmarker_t) {
        .enabled = true,
        .tile = tile_code,
        .color = color_code,
        .tile_pos = clamped_tile_pos(tile_pos)
    };
}

/*== GAMEPLAY CODE ===========================================================*/

// initialize the playfield tiles
static void game_init_playfield(void) {
    // MOVE THE TILEMAP UP 8 PIXELS TO ALLOW MAZE TO "FIT" + SET THE DEFAULT MAP TILES
    tilemap_scrollwrapclear( LOWER_LAYER, TM_CLEAR ); tilemap_scrollwrapclear( LOWER_LAYER, TM_UP, 8 );
    set_tilemap_bitamps_from_spritesheet( LOWER_LAYER, &tilemap_lower[0] );
    // decode the playfield from an ASCII map into tiles codes
    static const char* tiles =
       //0123456789012345678901234567
        "0UUUUUUUUUUUU45UUUUUUUUUUUU1" // 3
        "L............rl............R" // 4
        "L.ebbf.ebbbf.rl.ebbbf.ebbf.R" // 5
        "LPr  l.r   l.rl.r   l.r  lPR" // 6
        "L.guuh.guuuh.gh.guuuh.guuh.R" // 7
        "L..........................R" // 8
        "L.ebbf.ef.ebbbbbbf.ef.ebbf.R" // 9
        "L.guuh.rl.guuyxuuh.rl.guuh.R" // 10
        "L......rl....rl....rl......R" // 11
        "2BBBBf.rzbbf rl ebbwl.eBBBB3" // 12
        "     L.rxuuh gh guuyl.R     " // 13
        "     L.rl          rl.R     " // 14
        "     L.rl mjs--tjn rl.R     " // 15
        "UUUUUh.gh i      q gh.gUUUUU" // 16
        "      .   i      q   .      " // 17
        "BBBBBf.ef i      q ef.eBBBBB" // 18
        "     L.rl okkkkkkp rl.R     " // 19
        "     L.rl          rl.R     " // 20
        "     L.rl ebbbbbbf rl.R     " // 21
        "0UUUUh.gh guuyxuuh gh.gUUUU1" // 22
        "L............rl............R" // 23
        "L.ebbf.ebbbf.rl.ebbbf.ebbf.R" // 24
        "L.guyl.guuuh.gh.guuuh.rxuh.R" // 25
        "LP..rl.......  .......rl..PR" // 26
        "6bf.rl.ef.ebbbbbbf.ef.rl.eb8" // 27
        "7uh.gh.rl.guuyxuuh.rl.gh.gu9" // 28
        "L......rl....rl....rl......R" // 29
        "L.ebbbbwzbbf.rl.ebbwzbbbbf.R" // 30
        "L.guuuuuuuuh.gh.guuuuuuuuh.R" // 31
        "L..........................R" // 32
        "2BBBBBBBBBBBBBBBBBBBBBBBBBB3"; // 33
       //0123456789012345678901234567
    uint16_t t[128];
    for (int i = 0; i < 128; i++) { t[i]=TILE_DOT; }
    t[' ']=0x0000; t['0']=0x0001; t['1']=0x0101; t['2']=0x0201; t['3']=0x0301; t['4']=0x0003;
    t['5']=0x0103; t['6']=0x070b; t['7']=0x0703; t['8']=0x0503; t['9']=0x050b; t['U']=0x0002;
    t['L']=0x0702; t['R']=0x0502; t['B']=0x0202; t['b']=0x0004; t['e']=0x0005; t['f']=0x0105;
    t['g']=0x0205; t['h']=0x0305; t['l']=0x0504; t['r']=0x0704; t['u']=0x0204; t['w']=0x0305;
    t['x']=0x0005; t['y']=0x0105; t['z']=0x0205; t['m']=0x0007; t['n']=0x0107; t['o']=0x0207;
    t['p']=0x0307; t['j']=0x0202; t['i']=0x0502; t['k']=0x0002; t['q']=0x0702; t['s']=0x0108;
    t['t']=0x0008; t['-']=TILE_DOOR; t['P']=TILE_PILL;
    for (int y = 3, i = 0; y <= 33; y++) {
        for (int x = 0; x < 28; x++, i++) {
            state.gfx.video_ram[y][x] = t[tiles[i] & 127];
            set_tilemap_tile( LOWER_LAYER, x + 8, y - 1, t[tiles[i]]&0xff,(t[tiles[i]]&0xff00)>>8);
        }
    }
    gpu_line( 15, 48, 0, 271, 0 ); gpu_line( 15, 48, 239, 271, 239 );
}

// disable all game loop timers
static void game_disable_timers(void) {
    disable(&state.game.round_won);
    disable(&state.game.game_over);
    disable(&state.game.dot_eaten);
    disable(&state.game.pill_eaten);
    disable(&state.game.ghost_eaten);
    disable(&state.game.pacman_eaten);
    disable(&state.game.fruit_eaten);
    disable(&state.game.force_leave_house);
    disable(&state.game.fruit_active);
}

// one-time init at start of game state
static void game_init(void) {
    input_enable();
    game_disable_timers();
    state.game.round = DBG_START_ROUND;
    state.game.freeze = FREEZETYPE_PRELUDE;
    state.game.num_lives = NUM_LIVES;
    state.game.global_dot_counter_active = false;
    state.game.global_dot_counter = 0;
    state.game.num_dots_eaten = 0;
    state.game.score = 0;

    // draw the playfield and PLAYER ONE READY! message
    vid_clear(TILE_SPACE, COLOR_DOT);
    vid_color_text(i2(0,29),  COLOR_DEFAULT, "HS");
    game_init_playfield();
    vid_color_text(i2(15,11), COLOR_INKY, "PLAYER ONE");
    vid_color_text(i2(17, 17), COLOR_PACMAN, "READY!");
}

// setup state at start of a game round
static void game_round_init(void) {
    spr_clear();

    // clear the "PLAYER ONE" text
    vid_color_text(i2(15,11), 0x10, "          ");

    /* if a new round was started because Pacman has "won" (eaten all dots),
        redraw the playfield and reset the global dot counter
    */
    if (state.game.num_dots_eaten == NUM_DOTS) {
        state.game.round++;
        state.game.num_dots_eaten = 0;
        game_init_playfield();
        state.game.global_dot_counter_active = false;
    }
    else {
        /* if the previous round was lost, use the global dot counter
           to detect when ghosts should leave the ghost house instead
           of the per-ghost dot counter
        */
        if (state.game.num_lives != NUM_LIVES) {
            state.game.global_dot_counter_active = true;
            state.game.global_dot_counter = 0;
        }
        state.game.num_lives--;
    }

    state.game.active_fruit = FRUIT_NONE;
    state.game.freeze = FREEZETYPE_READY;
    state.game.xorshift = 0x12345678;   // random-number-generator seed
    state.game.num_ghosts_eaten = 0;
    game_disable_timers();

    vid_color_text(i2(17, 17), COLOR_PACMAN, "READY!");

    // the force-house timer forces ghosts out of the house if Pacman isn't
    // eating dots for a while
    start(&state.game.force_leave_house);

    // Pacman starts running to the left
    state.game.pacman = (pacman_t) {
        .actor = {
            .dir = DIR_LEFT,
            .pos = { 14*8, 26*8+4 },
        },
    };
    state.gfx.sprite[SPRITE_PACMAN] = (sprite_t) { .enabled = true, .color = COLOR_PACMAN };

    // Blinky starts outside the ghost house, looking to the left, and in scatter mode
    state.game.ghost[GHOSTTYPE_BLINKY] = (ghost_t) {
        .actor = {
            .dir = DIR_LEFT,
            .pos = ghost_starting_pos[GHOSTTYPE_BLINKY],
        },
        .type = GHOSTTYPE_BLINKY,
        .next_dir = DIR_LEFT,
        .state = GHOSTSTATE_SCATTER,
        .frightened = disabled_timer(),
        .eaten = disabled_timer(),
        .dot_counter = 0,
        .dot_limit = 0
    };
    state.gfx.sprite[SPRITE_BLINKY] = (sprite_t) { .enabled = true, .color = COLOR_BLINKY };

    // Pinky starts in the middle slot of the ghost house, moving down
    state.game.ghost[GHOSTTYPE_PINKY] = (ghost_t) {
        .actor = {
            .dir = DIR_DOWN,
            .pos = ghost_starting_pos[GHOSTTYPE_PINKY],
        },
        .type = GHOSTTYPE_PINKY,
        .next_dir = DIR_DOWN,
        .state = GHOSTSTATE_HOUSE,
        .frightened = disabled_timer(),
        .eaten = disabled_timer(),
        .dot_counter = 0,
        .dot_limit = 0
    };
    state.gfx.sprite[SPRITE_PINKY] = (sprite_t) { .enabled = true, .color = COLOR_PINKY };

    // Inky starts in the left slot of the ghost house moving up
    state.game.ghost[GHOSTTYPE_INKY] = (ghost_t) {
        .actor = {
            .dir = DIR_UP,
            .pos = ghost_starting_pos[GHOSTTYPE_INKY],
        },
        .type = GHOSTTYPE_INKY,
        .next_dir = DIR_UP,
        .state = GHOSTSTATE_HOUSE,
        .frightened = disabled_timer(),
        .eaten = disabled_timer(),
        .dot_counter = 0,
        // FIXME: needs to be adjusted by current round!
        .dot_limit = 30
    };
    state.gfx.sprite[SPRITE_INKY] = (sprite_t) { .enabled = true, .color = COLOR_INKY };

    // Clyde starts in the right slot of the ghost house, moving up
    state.game.ghost[GHOSTTYPE_CLYDE] = (ghost_t) {
        .actor = {
            .dir = DIR_UP,
            .pos = ghost_starting_pos[GHOSTTYPE_CLYDE],
        },
        .type = GHOSTTYPE_CLYDE,
        .next_dir = DIR_UP,
        .state = GHOSTSTATE_HOUSE,
        .frightened = disabled_timer(),
        .eaten = disabled_timer(),
        .dot_counter = 0,
        // FIXME: needs to be adjusted by current round!
        .dot_limit = 60,
    };
    state.gfx.sprite[SPRITE_CLYDE] = (sprite_t) { .enabled = true, .color = COLOR_CLYDE };
}

// update dynamic background tiles
static void game_update_tiles(void) {
    // print score and hiscore
    vid_color_score(i2(5,1), COLOR_DEFAULT, state.game.score);
    if (state.game.hiscore > 0) {
        vid_color_score(i2(5,28), COLOR_DEFAULT, state.game.hiscore);
    }

    // update the energizer pill colors (blinking/non-blinking)
    static const int2_t pill_pos[NUM_PILLS] = { { 1, 6 }, { 26, 6 }, { 1, 26 }, { 26, 26 } };
    for (int i = 0; i < NUM_PILLS; i++) {
        if (state.game.freeze) {
        }
        else {
            if( tile_code_at( pill_pos[i] ) != TILE_SPACE )
                paws_tile( pill_pos[i], (state.timing.tick & 0x8) ? TILE_PILL : TILE_PILL - 1 );
        }
    }

    // clear the fruit-eaten score after Pacman has eaten a bonus fruit
    if (after_once(state.game.fruit_eaten, 2*60)) {
        vid_fruit_score(FRUIT_NONE);
    }

    // remaining lives at left of screen
    for (int i = 0; i < NUM_LIVES; i++) {
        if(i < state.game.num_lives) {
            set_tilemap_32x32tile( UPPER_LAYER, 2, 5 + i*2, 17 );
        } else {
            set_tilemap_tile( UPPER_LAYER, 2, 6+i*2, 0, 0 );
            set_tilemap_tile( UPPER_LAYER, 2, 5+i*2, 0, 0 );
            set_tilemap_tile( UPPER_LAYER, 3, 5+i*2, 0, 0 );
            set_tilemap_tile( UPPER_LAYER, 3, 6+i*2, 0, 0 );
        }
    }

    // bonus fruit list at right of screen
    {
        int y = 28;
        for (int i = ((int)state.game.round - NUM_STATUS_FRUITS + 1); i <= (int)state.game.round; i++) {
            if (i >= 0) {
                fruit_t fruit = levelspec(i).bonus_fruit;
                set_tilemap_32x32tile( UPPER_LAYER, 40, y, 4*fruit_tiles_colors[fruit][0]+21 );
                y -= 2 ;
            }
        }
    }

    // if game round was won, render the entire playfield as blinking blue/white
    if (after(state.game.round_won, 1*60)) {
        if (since(state.game.round_won) & 0x10) {
            set_tilemap_bitamps_from_spritesheet( LOWER_LAYER, &tilemap_lower[0] );
            gpu_line( 15, 48, 0, 271, 0 ); gpu_line( 15, 48, 239, 271, 239 );
        } else {
            set_tilemap_bitamps_from_spritesheet( LOWER_LAYER, &tilemap_lower_alt[0] );
            gpu_line( 255, 48, 0, 271, 0 ); gpu_line( 255, 48, 239, 271, 239 );
        }
    }
}

// this function takes care of updating all sprite images during gameplay
static void game_update_sprites(void) {
    // update Pacman sprite
    {
        sprite_t* spr = spr_pacman();
        if (spr->enabled) {
            const actor_t* actor = &state.game.pacman.actor;
            spr->pos = actor_to_sprite_pos(actor->pos);
            if (state.game.freeze & FREEZETYPE_EAT_GHOST) {
                // hide Pacman shortly after he's eaten a ghost (via an invisible Sprite tile)
                spr->tile = SPRITETILE_INVISIBLE;
            }
            else if (state.game.freeze & (FREEZETYPE_PRELUDE|FREEZETYPE_READY)) {
                // special case game frozen at start of round, show Pacman with 'closed mouth'
                spr->tile = SPRITETILE_PACMAN_CLOSED_MOUTH;
            }
            else if (state.game.freeze & FREEZETYPE_DEAD) {
                // play the Pacman-death-animation after a short pause
                if (after(state.game.pacman_eaten, PACMAN_EATEN_TICKS)) {
                    spr_anim_pacman_death(since(state.game.pacman_eaten) - PACMAN_EATEN_TICKS);
                }
            }
            else {
                // regular Pacman animation
                spr_anim_pacman(actor->dir, actor->anim_tick);
            }
        }
    }

    // update ghost sprites
    for (int i = 0; i < NUM_GHOSTS; i++) {
        sprite_t* sprite = spr_ghost(i);
        if (sprite->enabled) {
            const ghost_t* ghost = &state.game.ghost[i];
            sprite->pos = actor_to_sprite_pos(ghost->actor.pos);
            // if Pacman has just died, hide ghosts
            if (state.game.freeze & FREEZETYPE_DEAD) {
                if (after(state.game.pacman_eaten, PACMAN_EATEN_TICKS)) {
                    sprite->tile = SPRITETILE_INVISIBLE;
                }
            }
            // if Pacman has won the round, hide ghosts
            else if (state.game.freeze & FREEZETYPE_WON) {
                sprite->tile = SPRITETILE_INVISIBLE;
            }
            else switch (ghost->state) {
                case GHOSTSTATE_EYES:
                    if (before(ghost->eaten, GHOST_EATEN_FREEZE_TICKS)) {
                        // if the ghost was *just* eaten by Pacman, the ghost's sprite
                        // is replaced with a score number for a short time
                        // (200 for the first ghost, followed by 400, 800 and 1600)
                        sprite->tile = SPRITETILE_SCORE_200 + state.game.num_ghosts_eaten - 1;
                        sprite->color = COLOR_GHOST_SCORE;
                    }
                    else {
                        // afterwards, the ghost's eyes are shown, heading back to the ghost house
                        spr_anim_ghost_eyes(i, ghost->next_dir);
                    }
                    break;
                case GHOSTSTATE_ENTERHOUSE:
                    // ...still show the ghost eyes while entering the ghost house
                    spr_anim_ghost_eyes(i, ghost->actor.dir);
                    break;
                case GHOSTSTATE_FRIGHTENED:
                    // when inside the ghost house, show the normal ghost images
                    // (FIXME: ghost's inside the ghost house also show the
                    // frightened appearance when Pacman has eaten an energizer pill)
                    spr_anim_ghost_frightened(i, since(ghost->frightened));
                    break;
                default:
                    // show the regular ghost sprite image, the ghost's
                    // 'next_dir' is used to visualize the direction the ghost
                    // is heading to, this has the effect that ghosts already look
                    // into the direction they will move into one tile ahead
                    spr_anim_ghost(i, ghost->next_dir, ghost->actor.anim_tick);
                    break;
            }
        }
    }

    // hide or display the currently active bonus fruit
    if (state.game.active_fruit == FRUIT_NONE) {
        spr_fruit()->enabled = false;
    }
    else {
        sprite_t* spr = spr_fruit();
        spr->enabled = true;
        spr->pos = i2(13 * TILE_WIDTH, 19 * TILE_HEIGHT + TILE_HEIGHT/2);
        spr->tile = fruit_tiles_colors[state.game.active_fruit][1];
        spr->color = fruit_tiles_colors[state.game.active_fruit][2];
    }
}

// return true if Pacman should move in this tick, when eating dots, Pacman
// is slightly slower than ghosts, otherwise slightly faster
static bool game_pacman_should_move(void) {
    if (now(state.game.dot_eaten)) {
        // eating a dot causes Pacman to stop for 1 tick
        return false;
    }
    else if (since(state.game.pill_eaten) < 3) {
        // eating an energizer pill causes Pacman to stop for 3 ticks
        return false;
    }
    else {
        return 0 != (state.timing.tick % 8);
    }
}

// return number of pixels a ghost should move this tick, this can't be a simple
// move/don't move boolean return value, because ghosts in eye state move faster
// than one pixel per tick
static int game_ghost_speed(const ghost_t* ghost) {
    switch (ghost->state) {
        case GHOSTSTATE_HOUSE:
        case GHOSTSTATE_LEAVEHOUSE:
            // inside house at half speed (estimated)
            return state.timing.tick & 1;
        case GHOSTSTATE_FRIGHTENED:
            // move at 50% speed when frightened
            return state.timing.tick & 1;
        case GHOSTSTATE_EYES:
        case GHOSTSTATE_ENTERHOUSE:
            // estimated 1.5x when in eye state, Pacman Dossier is silent on this
            return (state.timing.tick & 1) ? 1 : 2;
        default:
            if (is_tunnel(pixel_to_tile_pos(ghost->actor.pos))) {
                // move drastically slower when inside tunnel
                return ((state.timing.tick * 2) % 4) ? 1 : 0;
            }
            else {
                // otherwise move just a bit slower than Pacman
                return (state.timing.tick % 7) ? 1 : 0;
            }
    }
}

// return the current global scatter or chase phase
static ghoststate_t game_scatter_chase_phase(void) {
    uint32_t t = since(state.game.round_started);
    if (t < 7*60)       return GHOSTSTATE_SCATTER;
    else if (t < 27*60) return GHOSTSTATE_CHASE;
    else if (t < 34*60) return GHOSTSTATE_SCATTER;
    else if (t < 54*60) return GHOSTSTATE_CHASE;
    else if (t < 59*60) return GHOSTSTATE_SCATTER;
    else if (t < 79*60) return GHOSTSTATE_CHASE;
    else if (t < 84*60) return GHOSTSTATE_SCATTER;
    else return GHOSTSTATE_CHASE;
}

// this function takes care of switching ghosts into a new state, this is one
// of two important functions of the ghost AI (the other being the target selection
// function below)
static void game_update_ghost_state(ghost_t* ghost) {
    ghoststate_t new_state = ghost->state;
    switch (ghost->state) {
        case GHOSTSTATE_EYES:
            // When in eye state (heading back to the ghost house), check if the
            // target position in front of the ghost house has been reached, then
            // switch into ENTERHOUSE state. Since ghosts in eye state move faster
            // than one pixel per tick, do a fuzzy comparison with the target pos
            if (nearequal_i2(ghost->actor.pos, i2(ANTEPORTAS_X, ANTEPORTAS_Y), 1)) {
                new_state = GHOSTSTATE_ENTERHOUSE;
            }
            break;
        case GHOSTSTATE_ENTERHOUSE:
            // Ghosts that enter the ghost house during the gameplay loop immediately
            // leave the house again after reaching their target position inside the house.
            if (nearequal_i2(ghost->actor.pos, ghost_house_target_pos[ghost->type], 1)) {
                new_state = GHOSTSTATE_LEAVEHOUSE;
            }
            break;
        case GHOSTSTATE_HOUSE:
            // Ghosts only remain in the "house state" after a new game round
            // has been started. The conditions when ghosts leave the house
            // are a bit complicated, best to check the Pacman Dossier for the details.
            if (after_once(state.game.force_leave_house, 4*60)) {
                // if Pacman hasn't eaten dots for 4 seconds, the next ghost
                // is forced out of the house
                // FIXME: time is reduced to 3 seconds after round 5
                new_state = GHOSTSTATE_LEAVEHOUSE;
                start(&state.game.force_leave_house);
            }
            else if (state.game.global_dot_counter_active) {
                // if Pacman has lost a life this round, the global dot counter is used
                if ((ghost->type == GHOSTTYPE_PINKY) && (state.game.global_dot_counter == 7)) {
                    new_state = GHOSTSTATE_LEAVEHOUSE;
                }
                else if ((ghost->type == GHOSTTYPE_INKY) && (state.game.global_dot_counter == 17)) {
                    new_state = GHOSTSTATE_LEAVEHOUSE;
                }
                else if ((ghost->type == GHOSTTYPE_CLYDE) && (state.game.global_dot_counter == 32)) {
                    new_state = GHOSTSTATE_LEAVEHOUSE;
                    // NOTE that global dot counter is deactivated if (and only if) Clyde
                    // is in the house and the dot counter reaches 32
                    state.game.global_dot_counter_active = false;
                }
            }
            else if (ghost->dot_counter == ghost->dot_limit) {
                // in the normal case, check the ghost's personal dot counter
                new_state = GHOSTSTATE_LEAVEHOUSE;
            }
            break;
        case GHOSTSTATE_LEAVEHOUSE:
            // ghosts immediately switch to scatter mode after leaving the ghost house
            if (ghost->actor.pos.y == ANTEPORTAS_Y) {
                new_state = GHOSTSTATE_SCATTER;
            }
            break;
        default:
            // switch between frightened, scatter and chase mode
            if (before(ghost->frightened, levelspec(state.game.round).fright_ticks)) {
                new_state = GHOSTSTATE_FRIGHTENED;
            }
            else {
                new_state = game_scatter_chase_phase();
            }
    }
    // handle state transitions
    if (new_state != ghost->state) {
        switch (ghost->state) {
            case GHOSTSTATE_LEAVEHOUSE:
                // after leaving the ghost house, head to the left
                ghost->next_dir = ghost->actor.dir = DIR_LEFT;
                break;
            case GHOSTSTATE_ENTERHOUSE:
                // a ghost that was eaten is immune to frighten until Pacman eats enother pill
                disable(&ghost->frightened);
                break;
            case GHOSTSTATE_FRIGHTENED:
                // don't reverse direction when leaving frightened state
                break;
            case GHOSTSTATE_SCATTER:
            case GHOSTSTATE_CHASE:
                // any transition from scatter and chase mode causes a reversal of direction
                ghost->next_dir = reverse_dir(ghost->actor.dir);
                break;
            default:
                break;
        }
        ghost->state = new_state;
    }
}

// update the ghost's target position, this is the other important function
// of the ghost's AI
static void game_update_ghost_target(ghost_t* ghost) {
    int2_t pos = ghost->target_pos;
    switch (ghost->state) {
        case GHOSTSTATE_SCATTER:
            // when in scatter mode, each ghost heads to its own scatter
            // target position in the playfield corners
            pos = ghost_scatter_targets[ghost->type];
            break;
        case GHOSTSTATE_CHASE:
            // when in chase mode, each ghost has its own particular
            // chase behaviour (see the Pacman Dossier for details)
            {
                const actor_t* pm = &state.game.pacman.actor;
                const int2_t pm_pos = pixel_to_tile_pos(pm->pos);
                const int2_t pm_dir = dir_to_vec(pm->dir);
                switch (ghost->type) {
                    case GHOSTTYPE_BLINKY:
                        // Blinky directly chases Pacman
                        pos = pm_pos;
                        break;
                    case GHOSTTYPE_PINKY:
                        // Pinky target is 4 tiles ahead of Pacman
                        // FIXME: does not reproduce 'diagonal overflow'
                        pos = add_i2(pm_pos, mul_i2(pm_dir, 4));
                        break;
                    case GHOSTTYPE_INKY:
                        // Inky targets an extrapolated pos along a line two tiles
                        // ahead of Pacman through Blinky
                        {
                            const int2_t blinky_pos = pixel_to_tile_pos(state.game.ghost[GHOSTTYPE_BLINKY].actor.pos);
                            const int2_t p = add_i2(pm_pos, mul_i2(pm_dir, 2));
                            const int2_t d = sub_i2(p, blinky_pos);
                            pos = add_i2(blinky_pos, mul_i2(d, 2));
                        }
                        break;
                    case GHOSTTYPE_CLYDE:
                        // if Clyde is far away from Pacman, he chases Pacman,
                        // but if close he moves towards the scatter target
                        if (squared_distance_i2(pixel_to_tile_pos(ghost->actor.pos), pm_pos) > 64) {
                            pos = pm_pos;
                        }
                        else {
                            pos = ghost_scatter_targets[GHOSTTYPE_CLYDE];
                        }
                        break;
                    default:
                        break;
                }
            }
            break;
        case GHOSTSTATE_FRIGHTENED:
            // in frightened state just select a random target position
            // this has the effect that ghosts in frightened state
            // move in a random direction at each intersection
            pos = i2(xorshift32() % DISPLAY_TILES_X, xorshift32() % DISPLAY_TILES_Y);
            break;
        case GHOSTSTATE_EYES:
            // move towards the ghost house door
            pos = i2(13, 14);
            break;
        default:
            break;
    }
    ghost->target_pos = pos;
}

// compute the next ghost direction, return true if resulting movement
// should always happen regardless of current ghost position or blocking
// tiles (this special case is used for movement inside the ghost house)
static bool game_update_ghost_dir(ghost_t* ghost) {
    // inside ghost-house, just move up and down
    if (ghost->state == GHOSTSTATE_HOUSE) {
        if (ghost->actor.pos.y <= 17*TILE_HEIGHT) {
            ghost->next_dir = DIR_DOWN;
        }
        else if (ghost->actor.pos.y >= 18*TILE_HEIGHT) {
            ghost->next_dir = DIR_UP;
        }
        ghost->actor.dir = ghost->next_dir;
        // force movement
        return true;
    }
    // navigate the ghost out of the ghost house
    else if (ghost->state == GHOSTSTATE_LEAVEHOUSE) {
        const int2_t pos = ghost->actor.pos;
        if (pos.x == ANTEPORTAS_X) {
            if (pos.y > ANTEPORTAS_Y) {
                ghost->next_dir = DIR_UP;
            }
        }
        else {
            const int16_t mid_y = 17*TILE_HEIGHT + TILE_HEIGHT/2;
            if (pos.y > mid_y) {
                ghost->next_dir = DIR_UP;
            }
            else if (pos.y < mid_y) {
                ghost->next_dir = DIR_DOWN;
            }
            else {
                ghost->next_dir = (pos.x > ANTEPORTAS_X) ? DIR_LEFT:DIR_RIGHT;
            }
        }
        ghost->actor.dir = ghost->next_dir;
        return true;
    }
    // navigate towards the ghost house target pos
    else if (ghost->state == GHOSTSTATE_ENTERHOUSE) {
        const int2_t pos = ghost->actor.pos;
        const int2_t tile_pos = pixel_to_tile_pos(pos);
        const int2_t tgt_pos = ghost_house_target_pos[ghost->type];
        if (tile_pos.y == 14) {
            if (pos.x != ANTEPORTAS_X) {
                ghost->next_dir = (pos.x < ANTEPORTAS_X) ? DIR_RIGHT:DIR_LEFT;
            }
            else {
                ghost->next_dir = DIR_DOWN;
            }
        }
        else if (pos.y == tgt_pos.y) {
            ghost->next_dir = (pos.x < tgt_pos.x) ? DIR_RIGHT:DIR_LEFT;
        }
        ghost->actor.dir = ghost->next_dir;
        return true;
    }
    // scatter/chase/frightened: just head towards the current target point
    else {
        // only compute new direction when currently at midpoint of tile
        const int2_t dist_to_mid = dist_to_tile_mid(ghost->actor.pos);
        if ((dist_to_mid.x == 0) && (dist_to_mid.y == 0)) {
            // new direction is the previously computed next-direction
            ghost->actor.dir = ghost->next_dir;

            // compute new next-direction
            const int2_t dir_vec = dir_to_vec(ghost->actor.dir);
            const int2_t lookahead_pos = add_i2(pixel_to_tile_pos(ghost->actor.pos), dir_vec);

            // try each direction and take the one that moves closest to the target
            const dir_t dirs[NUM_DIRS] = { DIR_UP, DIR_LEFT, DIR_DOWN, DIR_RIGHT };
            int min_dist = 100000;
            int dist = 0;
            for (int i = 0; i < NUM_DIRS; i++) {
                const dir_t dir = dirs[i];
                // if ghost is in one of the two 'red zones', forbid upward movement
                // (see Pacman Dossier "Areas To Exploit")
                if (is_redzone(lookahead_pos) && (dir == DIR_UP) && (ghost->state != GHOSTSTATE_EYES)) {
                    continue;
                }
                const dir_t revdir = reverse_dir(dir);
                const int2_t test_pos = clamped_tile_pos(add_i2(lookahead_pos, dir_to_vec(dir)));
                if ((revdir != ghost->actor.dir) && !is_blocking_tile(test_pos)) {
                    if ((dist = squared_distance_i2(test_pos, ghost->target_pos)) < min_dist) {
                        min_dist = dist;
                        ghost->next_dir = dir;
                    }
                }
            }
        }
        return false;
    }
}

/* Update the dot counters used to decide whether ghosts must leave the house.

    This is called each time Pacman eats a dot.

    Each ghost has a dot limit which is reset at the start of a round. Each time
    Pacman eats a dot, the highest priority ghost in the ghost house counts
    down its dot counter.

    When the ghost's dot counter reaches zero the ghost leaves the house
    and the next highest-priority dot counter starts counting.

    If a life is lost, the personal dot counters are deactivated and instead
    a global dot counter is used.

    If pacman doesn't eat dots for a while, the next ghost is forced out of the
    house using a timer.
*/
static void game_update_ghosthouse_dot_counters(void) {
    // if the new round was started because Pacman lost a life, use the global
    // dot counter (this mode will be deactivated again after all ghosts left the
    // house)
    if (state.game.global_dot_counter_active) {
        state.game.global_dot_counter++;
    }
    else {
        // otherwise each ghost has his own personal dot counter to decide
        // when to leave the ghost house
        for (int i = 0; i < NUM_GHOSTS; i++) {
            if (state.game.ghost[i].dot_counter < state.game.ghost[i].dot_limit) {
                state.game.ghost[i].dot_counter++;
                break;
            }
        }
    }
}

// called when a dot or pill has been eaten, checks if a round has been won
// (all dots and pills eaten), whether to show the bonus fruit, and finally
// plays the dot-eaten sound effect
static void game_update_dots_eaten(void) {
    state.game.num_dots_eaten++;
    if (state.game.num_dots_eaten == NUM_DOTS) {
        // all dots eaten, round won
        start(&state.game.round_won);
        paws_snd( SND_STOP_ALL);
    }
    else if ((state.game.num_dots_eaten == 70) || (state.game.num_dots_eaten == 170)) {
        // at 70 and 170 dots, show the bonus fruit
        start(&state.game.fruit_active);
    }

    // play crunch sound effect when a dot has been eaten
    // play alternating crunch sound effect when a dot has been eaten
    if (state.game.num_dots_eaten & 1) {
        paws_snd( SND_START_DOT1 );
    } else {
        paws_snd( SND_START_DOT2 );
    }
}

// the central Pacman and ghost behaviour function, called once per game tick
static void game_update_actors(void) {
    // Pacman "AI"
    if (game_pacman_should_move()) {
        // move Pacman with cornering allowed
        actor_t* actor = &state.game.pacman.actor;
        const dir_t wanted_dir = input_dir(actor->dir);
        const bool allow_cornering = true;
        // look ahead to check if the wanted direction is blocked
        if (can_move(actor->pos, wanted_dir, allow_cornering)) {
            actor->dir = wanted_dir;
        }
        // move into the selected direction
        if (can_move(actor->pos, actor->dir, allow_cornering)) {
            actor->pos = move(actor->pos, actor->dir, allow_cornering);
            actor->anim_tick++;
        }
        // eat dot or energizer pill?
        const int2_t tile_pos = pixel_to_tile_pos(actor->pos);
        if (is_dot(tile_pos)) {
            paws_tile(tile_pos, TILE_SPACE);
            state.game.score += 1;
            start(&state.game.dot_eaten);
            start(&state.game.force_leave_house);
            game_update_dots_eaten();
            game_update_ghosthouse_dot_counters();
        }
        if (is_pill(tile_pos)) {
            paws_tile(tile_pos, TILE_SPACE);
            state.game.score += 5;
            game_update_dots_eaten();
            start(&state.game.pill_eaten);
            state.game.num_ghosts_eaten = 0;
            for (int i = 0; i < NUM_GHOSTS; i++) {
                start(&state.game.ghost[i].frightened);
            }
            paws_snd( SND_START_FRIGHTENDED );
        }
        // check if Pacman eats the bonus fruit
        if (state.game.active_fruit != FRUIT_NONE) {
            const int2_t test_pos = pixel_to_tile_pos(add_i2(actor->pos, i2(TILE_WIDTH/2, 0)));
            if (equal_i2(test_pos, i2(14, 20))) {
                start(&state.game.fruit_eaten);
                uint32_t score = levelspec(state.game.round).bonus_score;
                state.game.score += score;
                vid_fruit_score(state.game.active_fruit);
                state.game.active_fruit = FRUIT_NONE;
                paws_snd( SND_START_FRUIT );
            }
        }
        // check if Pacman collides with any ghost
        for (int i = 0; i < NUM_GHOSTS; i++) {
            ghost_t* ghost = &state.game.ghost[i];
            const int2_t ghost_tile_pos = pixel_to_tile_pos(ghost->actor.pos);
            if (equal_i2(tile_pos, ghost_tile_pos)) {
                if (ghost->state == GHOSTSTATE_FRIGHTENED) {
                    // Pacman eats a frightened ghost
                    ghost->state = GHOSTSTATE_EYES;
                    start(&ghost->eaten);
                    start(&state.game.ghost_eaten);
                    state.game.num_ghosts_eaten++;
                    // increase score by 20, 40, 80, 160
                    state.game.score += 10 * (1<<state.game.num_ghosts_eaten);
                    state.game.freeze |= FREEZETYPE_EAT_GHOST;
                    paws_snd( SND_START_GHOST );
                }
                else if ((ghost->state == GHOSTSTATE_CHASE) || (ghost->state == GHOSTSTATE_SCATTER)) {
                    // otherwise, ghost eats Pacman, Pacman loses a life
                    #if !DBG_GODMODE
                    paws_snd( SND_STOP_ALL);
                    start(&state.game.pacman_eaten);
                    state.game.freeze |= FREEZETYPE_DEAD;
                    // if Pacman has any lives left start a new round, otherwise start the game-over sequence
                    if (state.game.num_lives > 0) {
                        start_after(&state.game.ready_started, PACMAN_EATEN_TICKS+PACMAN_DEATH_TICKS);
                    }
                    else {
                        start_after(&state.game.game_over, PACMAN_EATEN_TICKS+PACMAN_DEATH_TICKS);
                    }
                    #endif
                }
            }
        }
    }

    // Ghost "AIs"
    for (int ghost_index = 0; ghost_index < NUM_GHOSTS; ghost_index++) {
        ghost_t* ghost = &state.game.ghost[ghost_index];
        // handle ghost-state transitions
        game_update_ghost_state(ghost);
        // update the ghost's target position
        game_update_ghost_target(ghost);
        // finally, move the ghost towards the current target position
        const int num_move_ticks = game_ghost_speed(ghost);
        for (int i = 0; i < num_move_ticks; i++) {
            bool force_move = game_update_ghost_dir(ghost);
            actor_t* actor = &ghost->actor;
            const bool allow_cornering = false;
            if (force_move || can_move(actor->pos, actor->dir, allow_cornering)) {
                actor->pos = move(actor->pos, actor->dir, allow_cornering);
                actor->anim_tick++;
            }
        }
    }
}

// the central game tick function, called at 60 Hz
static void game_tick(void) {
    // debug: skip prelude
    #if DBG_SKIP_PRELUDE
        const int prelude_ticks_per_sec = 1;
    #else
        const int prelude_ticks_per_sec = 60;
    #endif

    // initialize game state once
    if (now(state.game.started)) {
        start(&state.gfx.fadein);
        start_after(&state.game.ready_started, 2*prelude_ticks_per_sec);
        paws_snd( SND_START_INTRO );
        game_init();
    }
    // initialize new round (each time Pacman looses a life), make actors visible, remove "PLAYER ONE", start a new life
    if (now(state.game.ready_started)) {
        game_round_init();
        // after 2 seconds start the interactive game loop
        start_after(&state.game.round_started, 2*60+10);
    }
    if (now(state.game.round_started)) {
        state.game.freeze &= ~FREEZETYPE_READY;
        // clear the 'READY!' message
        vid_color_text(i2(17,17), 0x10, "      ");
        paws_snd( SND_START_NORMAL );
    }

    // activate/deactivate bonus fruit
    if (now(state.game.fruit_active)) {
        state.game.active_fruit = levelspec(state.game.round).bonus_fruit;
    }
    else if (after_once(state.game.fruit_active, FRUITACTIVE_TICKS)) {
        state.game.active_fruit = FRUIT_NONE;
    }

    // stop frightened sound and start weeooh sound
    if (after_once(state.game.pill_eaten, levelspec(state.game.round).fright_ticks)) {
        paws_snd( SND_START_NORMAL );
    }

    // if game is frozen because Pacman ate a ghost, unfreeze after a while
    if (state.game.freeze & FREEZETYPE_EAT_GHOST) {
        if (after_once(state.game.ghost_eaten, GHOST_EATEN_FREEZE_TICKS)) {
            state.game.freeze &= ~FREEZETYPE_EAT_GHOST;
        }
    }

    // play pacman-death sound
    if (after_once(state.game.pacman_eaten, PACMAN_EATEN_TICKS)) {
        paws_snd( SND_START_PACMAN );
    }

    // the actually important part: update Pacman and ghosts, update dynamic
    // background tiles, and update the sprite images
    if (!state.game.freeze) {
        game_update_actors();
    }
    game_update_tiles();
    game_update_sprites();

    // update hiscore
    if (state.game.score > state.game.hiscore) {
        state.game.hiscore = state.game.score;
    }

    // check for end-round condition
    if (now(state.game.round_won)) {
        state.game.freeze |= FREEZETYPE_WON;
        start_after(&state.game.ready_started, ROUNDWON_TICKS);
    }
    if (now(state.game.game_over)) {
        // display game over string
        vid_color_text(i2(15,17), COLOR_BLINKY, "GAME OVER!");
        input_disable();
        start_after(&state.gfx.fadeout, GAMEOVER_TICKS);
        start_after(&state.intro.started, GAMEOVER_TICKS+FADE_TICKS);
    }

    #if DBG_ESCAPE
        if (state.input.esc) {
            input_disable();
            start(&state.gfx.fadeout);
            start_after(&state.intro.started, FADE_TICKS);
        }
    #endif

    #if DBG_MARKERS
        // visualize current ghost targets
        for (int i = 0; i < NUM_GHOSTS; i++) {
            const ghost_t* ghost = &state.game.ghost[i];
            uint8_t tile = 'X';
            switch (ghost->state) {
                case GHOSTSTATE_NONE:       tile = 'N'; break;
                case GHOSTSTATE_CHASE:      tile = 'C'; break;
                case GHOSTSTATE_SCATTER:    tile = 'S'; break;
                case GHOSTSTATE_FRIGHTENED: tile = 'F'; break;
                case GHOSTSTATE_EYES:       tile = 'E'; break;
                case GHOSTSTATE_HOUSE:      tile = 'H'; break;
                case GHOSTSTATE_LEAVEHOUSE: tile = 'L'; break;
                case GHOSTSTATE_ENTERHOUSE: tile = 'E'; break;
            }
            dbg_marker(i, state.game.ghost[i].target_pos, tile, COLOR_BLINKY+2*i);
        }
    #endif
}

/*== INTRO GAMESTATE CODE ====================================================*/

static void intro_tick(void) {

    // on intro-state enter, enable input and draw any initial text
    if (now(state.intro.started)) {
        paws_snd( SND_STOP_ALL );
        spr_clear();
        start(&state.gfx.fadein);
        input_enable();
        vid_clear(TILE_SPACE, COLOR_DEFAULT);
        vid_color_text(i2(0,0),  COLOR_DEFAULT, "1UP");
        vid_color_text(i2(0,29),  COLOR_DEFAULT, "HS");
        vid_color_text(i2(37,0),  COLOR_DEFAULT, "2UP");
        vid_color_score(i2(5,1), COLOR_DEFAULT, 0);
        if (state.game.hiscore > 0) {
            vid_color_score(i2(5,28), COLOR_DEFAULT, state.game.hiscore);
        }
        vid_color_text(i2(13,3),  COLOR_DEFAULT, "CHARACTER / NICKNAME");
        vid_color_text(i2(34,29), COLOR_DEFAULT, "CREDIT");
    }

    // draw the animated 'ghost image.. name.. nickname' lines
    uint32_t delay = 30;
    const char* names[] = { "-SHADOW", "-SPEEDY", "-BASHFUL", "-POKEY" };
    const char* nicknames[] = { "BLINKY", "PINKY", "INKY", "CLYDE" };
    const char colours[] = { COLOR_BLINKY, COLOR_PINKY, COLOR_INKY, COLOR_CLYDE };

    for (int i = 0; i < 4; i++) {
        const uint8_t y = 3*i + 6;
        delay += 30;
        if (after_once(state.intro.started, delay)) {
            set_tilemap_32x32tile( UPPER_LAYER, 12, y+2, i*4+1 );
        }
        // after 1 second, the name of the ghost
        delay += 60;
        if (after_once(state.intro.started, delay)) {
            vid_color_text(i2(13,y+1), colours[i], names[i]);
        }
        // after 0.5 seconds, the nickname of the ghost
        delay += 30;
        if (after_once(state.intro.started, delay)) {
            vid_color_text(i2(23,y+1), colours[i], nicknames[i]);
        }
    }

    // . 10 PTS
    // O 50 PTS
    delay += 60;
    if (after_once(state.intro.started, delay)) {
        paws_tile(i2(10,24), TILE_DOT);
        vid_color_text(i2(18,21), COLOR_DEFAULT, "10 pts");
        paws_tile(i2(10,26), TILE_PILL);
        vid_color_text(i2(18,23), COLOR_DEFAULT, "50 pts");
    }

    // blinking "press any key" text
    delay += 60;
    if (after(state.intro.started, delay)) {
        if (since(state.intro.started) & 0x20) {
            vid_color_text(i2(9,27), COLOR_DEFAULT, "                       ");
        }
        else {
            vid_color_text(i2(9,27), COLOR_PINKY, "PRESS ANY KEY TO START!");
        }
    }

    // FIXME: animated chase sequence

    // if a key is pressed, advance to game state
    if (state.input.anykey) {
        input_disable();
        start(&state.gfx.fadeout);
        start_after(&state.game.started, FADE_TICKS);
    }
}

/*== GFX SUBSYSTEM ===========================================================*/

static void gfx_init(void) {
    set_tilemap_bitamps_from_spritesheet( LOWER_LAYER, &tilemap_lower[0] );
    set_sprite_bitamps_from_spritesheet( UPPER_LAYER, &sprite_upper[0] );

    for( int i = 0; i < 13; i++ )
        set_tilemap_bitmap32x32( UPPER_LAYER, i*4+1, &tilemap_upper[i*1024] );

    spr_clear();
}

static void gfx_shutdown(void) {
}

static void gfx_add_sprite_vertices(void) {
    sprite_t* spr; int action, basesprite,tile;
    actor_t* actor = &state.game.pacman.actor;

    for( int i = SPRITE_PACMAN; i <= SPRITE_FRUIT; i++ ) {
        spr = &state.gfx.sprite[ i ];
        switch( i ) {
            case SPRITE_PACMAN:
                // PACMAN IS PAWS SPRITE 14 and 15, 3 tiles, half open, full open, closed (facing right) generate remainder by rotation, then death over remaining tiles
                action = SPRITE_DOUBLE; basesprite = 15;
                switch(actor->dir) {
                    case 0: break;
                    case 1: action += ROTATE90; break;
                    case 2: action += ROTATE180; break;
                    case 3: action += ROTATE270; break;
                }
                switch(spr->tile) {
                    case SPRITETILE_PACMAN_CLOSED_MOUTH:
                        tile = 2;
                        break;
                    default:
                        tile = ( actor->anim_tick&1);
                        break;
                    case 52: case 53: case 54: case 55: case 56:
                        tile = spr->tile - 50; action = SPRITE_DOUBLE;
                        break;
                    case 57: case 58: case 59: case 60: case 61: case 62: case 63:
                        basesprite = 14; tile = spr->tile - 57; action = SPRITE_DOUBLE;
                        break;
                }
                set_sprite( UPPER_LAYER, basesprite, spr->enabled && (spr->tile != SPRITETILE_INVISIBLE), tocoords(spr->pos.x, spr->pos.y), tile, action );
                set_sprite_attribute( UPPER_LAYER, (basesprite==15) ? 14: 15, SPRITE_ACTIVE, FALSE );
                break;
            case SPRITE_BLINKY:
            case SPRITE_PINKY:
            case SPRITE_INKY:
            case SPRITE_CLYDE:
                basesprite = i-1; ghost_t* ghost = &state.game.ghost[i-1];
                if (spr->enabled && (spr->tile != SPRITETILE_INVISIBLE)) {
                    action = SPRITE_DOUBLE;
                    switch (ghost->state) {
                        case GHOSTSTATE_EYES:
                            set_sprite( UPPER_LAYER, basesprite+4, spr->color != COLOR_GHOST_SCORE, tocoords(spr->pos.x, spr->pos.y), ghost->next_dir + 4, action );
                            set_sprite_attribute( UPPER_LAYER, basesprite, SPRITE_ACTIVE, 0 );
                            set_sprite( UPPER_LAYER, basesprite+8, spr->color == COLOR_GHOST_SCORE, tocoords(spr->pos.x, spr->pos.y), spr->tile, action );
                            break;
                        case GHOSTSTATE_ENTERHOUSE:
                            set_sprite( UPPER_LAYER, basesprite+4, TRUE, tocoords(spr->pos.x, spr->pos.y), ghost->actor.dir + 4, action );
                            set_sprite_attribute( UPPER_LAYER, basesprite, SPRITE_ACTIVE, 0 );
                            set_sprite_attribute( UPPER_LAYER, basesprite+8, SPRITE_ACTIVE, 0 );
                            break;
                        case GHOSTSTATE_FRIGHTENED:
                            set_sprite( UPPER_LAYER, basesprite+4, TRUE, tocoords(spr->pos.x, spr->pos.y), (spr->color == COLOR_FRIGHTENED_BLINKING) ? 2 : 0 + (ghost->actor.anim_tick&1), action );
                            set_sprite_attribute( UPPER_LAYER, basesprite, SPRITE_ACTIVE, 0 );
                            set_sprite_attribute( UPPER_LAYER, basesprite+8, SPRITE_ACTIVE, 0 );
                            break;
                        default:
                            set_sprite( UPPER_LAYER, basesprite, TRUE, tocoords(spr->pos.x, spr->pos.y), ghost->next_dir * 2 + (ghost->actor.anim_tick&1), action );
                            set_sprite_attribute( UPPER_LAYER, basesprite+4, SPRITE_ACTIVE, 0 );
                            set_sprite_attribute( UPPER_LAYER, basesprite+8, SPRITE_ACTIVE, 0 );
                    }
                } else {
                    set_sprite_attribute( UPPER_LAYER, basesprite, SPRITE_ACTIVE, 0 );
                    set_sprite_attribute( UPPER_LAYER, basesprite+4, SPRITE_ACTIVE, 0 );
                    set_sprite_attribute( UPPER_LAYER, basesprite+8, SPRITE_ACTIVE, 0 );
                }
                break;
            case SPRITE_FRUIT:
                action = SPRITE_DOUBLE;
                set_sprite( UPPER_LAYER, 12, spr->enabled, tocoords(spr->pos.x, spr->pos.y), state.game.active_fruit - 1, action );
                break;
        }
    }
}


static void gfx_draw(void) {
    // update the playfield and sprite vertex buffer
    state.gfx.num_vertices = 0;
    gfx_add_sprite_vertices();
}

/*== AUDIO SUBSYSTEM =========================================================*/
static void paws_snd( int action ) {
    switch( action ) {
        case SND_START_INTRO:
            tune_upload( CHANNEL_LEFT, 64, &tune_treble[0] ); tune_upload( CHANNEL_RIGHT, 32, &tune_bass[0] );
            set_volume( 7, 7 );
            beep( CHANNEL_LEFT, WAVE_TUNE | WAVE_SINE, 0, 8 << 3 );
            beep( CHANNEL_RIGHT, WAVE_TUNE | WAVE_SINE, 0, 16 << 3 );
            break;
        case SND_START_DOT1:
            tune_upload( CHANNEL_RIGHT, 6, &eat_dot_1[0] );
            beep( CHANNEL_RIGHT, WAVE_TUNE | WAVE_SQUARE, 0, 16 );
            break;
        case SND_START_DOT2:
            tune_upload( CHANNEL_RIGHT, 6, &eat_dot_2[0] );
            beep( CHANNEL_RIGHT, WAVE_TUNE | WAVE_SQUARE, 0, 16 );
            break;
        case SND_START_FRUIT:
            tune_upload( CHANNEL_RIGHT, 24, &eat_fruit[0] );
            beep( CHANNEL_RIGHT, WAVE_TUNE | WAVE_SQUARE, 0, 16 );
            break;
        case SND_START_GHOST:
            tune_upload( CHANNEL_RIGHT, 33, &eat_ghost[0] );
            beep( CHANNEL_RIGHT, WAVE_TUNE | WAVE_SQUARE, 0, 16 );
            break;
        case SND_START_PACMAN:
            tune_upload( CHANNEL_RIGHT, 90, &eat_pacman[0] );
            beep( CHANNEL_RIGHT, WAVE_TUNE | WAVE_SQUARE, 0, 16 );
            break;
        case SND_START_NORMAL:
            set_volume( 6, 7 );
            tune_upload( CHANNEL_LEFT, 22, &alert_normal[0] );
            beep( CHANNEL_LEFT, SAMPLE_REPEAT | WAVE_TUNE | WAVE_SINE, 0, 16 );
            break;
        case SND_START_FRIGHTENDED:
            set_volume( 6, 7 );
            tune_upload( CHANNEL_LEFT, 8, &alert_frightended[0] );
            beep( CHANNEL_LEFT, SAMPLE_REPEAT | WAVE_TUNE | WAVE_SINE, 0, 16 );
            break;
        case SND_STOP_ALL:
            set_volume( 7, 7 );
            beep( CHANNEL_BOTH, 0, 0, 0 );
            break;
    }
}

static void snd_shutdown(void) {
      paws_snd( SND_STOP_ALL );
}


int main( int argc, char **argv ) {
    init();
    screen_mode( 0, MODE_RGBM, CM_LOW );

    // DISPLAY WELCOME SCREEN
    gpu_pixelblock( 0, 0, 320, 240, TRANSPARENT, pacman3dbitmap );
    tpu_set( 0, 27, TRANSPARENT, WHITE ); tpu_outputstring( TRUE, "Ported from" );
    tpu_set( 0, 28, TRANSPARENT, WHITE ); tpu_outputstring( FALSE, "https://github.com/floooh/pacman.c" );
    tpu_set( 0, 29, TRANSPARENT, WHITE ); tpu_outputstring( TRUE, "by Andre Weissflog" );
    sleep1khz( 4000, 0 );
    tpu_cs(); gpu_cs();

    while(1) {
        await_vblank();
        input();
        frame();
        await_vblank_finish();
    }
}
