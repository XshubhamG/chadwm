/* See LICENSE file for copyright and license details. */

#include <X11/X.h>
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx = 2; /* border pixel of windows */
static const unsigned int default_border = 2;
// to switch back to default border after dynamic border resizing via keybinds

static const unsigned int snap = 32;   /* snap pixel */
static const unsigned int gappih = 10; /* horiz inner gap between windows */
static const unsigned int gappiv = 10; /* vert inner gap between windows */

// horiz outer gap between windows and screen edge
static const unsigned int gappoh = 12;

// vert outer gap between windows and screen edge
static const unsigned int gappov = 12;

// 1 means no outer gap when there is only one window
static const int smartgaps = 0;

//  0: sloppy systray follows selected monitor, >0: pin systray to monitor X
static const unsigned int systraypinning = 0;

static const unsigned int systrayspacing = 2; /* systray spacing */
//  1: if pinning fails,display systray on the 1st monitor,False: display
//  systray on last monitor
static const int systraypinningfailfirst = 1;
static const int showsystray = 1; /* 0 means no systray */

static const int showbar = 1; /* 0 means no bar */
static const int showtab = showtab_auto;
static const int toptab = 1; /* 0 means bottom tab */
// 1 means the bar will float,0 means the bar have padding
static const int floatbar = 1;

static const int topbar = 1; /* 0 means bottom bar */
static const int horizpadbar = 5;
static const int vertpadbar = 5;

static const int vertpadtab = 35;
static const int horizpadtabi = 15;
static const int horizpadtabo = 15;

static const int scalepreview = 3;
static const int tag_preview = 0; /* 1 means enable, 0 is off */
static const int colorfultag = 1; /* 0 use SchemeSel for non vacant tag */

static const char *upvol[] = {"/usr/bin/pactl", "set-sink-volume", "0", "+5%",
                              NULL};
static const char *downvol[] = {"/usr/bin/pactl", "set-sink-volume", "0", "-5%",
                                NULL};
static const char *mutevol[] = {"/usr/bin/pactl", "set-sink-mute", "0",
                                "toggle", NULL};

static const char *clipboardcmd[] = {"/usr/lib/gpaste/gpaste-ui", NULL};

static const int new_window_attach_on_end = 0;

#define ICONSIZE 24    /* icon size */
#define ICONSPACING 14 /* space between icon and title */

static const char *fonts[] = {"JetBrainsMono Nerd Font:style:medium:size=14"};

// theme
#include "themes/catppuccin.h"

static const char *colors[][3] = {
    /*                     fg       bg      border */
    [SchemeNorm] = {gray3, black, gray2},
    [SchemeSel] = {gray4, blue, blue},
    [SchemeTitle] = {white, black, black}, // active window title
    [TabSel] = {blue, gray2, black},
    [TabNorm] = {gray3, black, black},
    [SchemeTag] = {gray3, black, black},
    [SchemeTag1] = {blue, black, black},
    [SchemeTag2] = {red, black, black},
    [SchemeTag3] = {orange, black, black},
    [SchemeTag4] = {green, black, black},
    [SchemeTag5] = {pink, black, black},
    [SchemeTag6] = {yellow, black, black},
    [SchemeLayout] = {green, black, black},
    [SchemeBtnPrev] = {green, black, black},
    [SchemeBtnNext] = {yellow, black, black},
    [SchemeBtnClose] = {red, black, black},
};

/* tagging */
static char *tags[] = {" 󰄛 ", "  ", "  ", " 󰓇 ", "  ", "  "};

static const int tagschemes[] = {SchemeTag1, SchemeTag2, SchemeTag3,
                                 SchemeTag4, SchemeTag5, SchemeTag6};

/* horizontal padding between the underline and tag */
static const unsigned int ulinepad = 5;
/* thickness / height of the underline */
static const unsigned int ulinestroke = 2;
/* how far above the bottom of the bar the line should appear */
static const unsigned int ulinevoffset = 0;
/* 1 to show underline on all tags, 0 for just the active ones */
static const int ulineall = 0;

static const Rule rules[] = {
    /*
     xprop(1):
     WM_CLASS(STRING) = instance, class
     WM_NAME(STRING) = title
    */

    /* class instance title winType tagsmask iscentered isfloating monitor */

    {"nwg-look", NULL, NULL, NULL, 1 << 4, 1, 1, -1}, /* nwg-look */
    {"qt6ct", NULL, NULL, NULL, 1 << 4, 1, 1, -1},    /* qt6ct */

    {"kitty", NULL, NULL, NULL, 1 << 0, 0, 0, -1},
    {"St", NULL, NULL, NULL, 0, 0, 0, -1},
    {"Brave-browser", NULL, NULL, NULL, 1 << 1, 0, 0, -1},
    {"zen", NULL, NULL, NULL, 1 << 1, 0, 0, -1},
    {"Cursor", NULL, NULL, NULL, 1 << 2, 0, 0, -1},
    {"spotify", NULL, NULL, NULL, 1 << 3, 0, 0, -1},
    {"Thunar", NULL, NULL, NULL, 1 << 5, 0, 0, -1},

};

/* layout(s) */
static const float mfact = 0.50; /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;    /* number of clients in master area */

/* 1 means respect size hints in tiled resizals */
static const int resizehints = 0;
/* 1 will force focus on the fullscreen window */
static const int lockfullscreen = 1;

#define FORCE_VSPLIT                                                           \
  1 /* nrowgrid layout: force two clients to always split vertically */
#include "functions.h"

static const Layout layouts[] = {
    {"[]", dwindle}, /* first entry is default */
    {"[]=", tile},           {"[M]", monocle},
    {"[@]", spiral},         {"H[]", deck},
    {"TTT", bstack},         {"===", bstackhoriz},
    {"HHH", grid},           {"###", nrowgrid},
    {"|M|", centeredmaster}, {">M>", centeredfloatingmaster},
    {"><>", NULL},           {NULL, NULL},
};

/* key definitions */
#define MODKEY Mod4Mask
#define ALTKEY Mod1Mask

#define TAGKEYS(KEY, TAG)                                                      \
  {MODKEY, KEY, view, {.ui = 1 << TAG}},                                       \
      {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},               \
      {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},                        \
      {MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd)                                                             \
  {                                                                            \
    .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                       \
  }

/* commands */

static const Key keys[] = {
    /* modifier                         key         function        argument
     */

    // brightness and audio
    {0, XF86XK_AudioLowerVolume, spawn, {.v = downvol}},
    {0, XF86XK_AudioMute, spawn, {.v = mutevol}},
    {0, XF86XK_AudioRaiseVolume, spawn, {.v = upvol}},
    {0, XF86XK_MonBrightnessUp, spawn,
     SHCMD("brightnessctl -d \"intel_backlight\" set 5%+")},
    {0, XF86XK_MonBrightnessDown, spawn,
     SHCMD("brightnessctl -d \"intel_backlight\" set 5%-")},

    // screenshot fullscreen and cropped
    {MODKEY, XK_s, spawn,
     SHCMD("scrot -f ~/Pictures/Screenshots/$(date +%d-%m-%Y)-Full.png ")},
    {MODKEY | ShiftMask, XK_s, spawn,
     SHCMD("scrot -s -f ~/Pictures/Screenshots/$(date +%Y-%m-%d).png")},
    {ALTKEY, XK_s, spawn, SHCMD("~/.local/bin/screenshot.sh")},

    {MODKEY, XK_r, spawn,
     SHCMD("rofi -show drun -theme themes/application.rasi")},
    {ALTKEY, XK_e, spawn, SHCMD("rofi -show emoji -theme themes/emoji.rasi")},

    {MODKEY | ShiftMask, XK_l, spawn, SHCMD("betterlockscreen -l dimblur")},
    {MODKEY, XK_Return, spawn, SHCMD("kitty")},
    {MODKEY, XK_e, spawn, SHCMD("thunar")},
    {MODKEY, XK_b, spawn, SHCMD("brave")},
    {MODKEY, XK_z, spawn, SHCMD("zen-browser")},
    {MODKEY, XK_c, spawn, SHCMD("cursor")},
    {MODKEY, XK_v, spawn, {.v = clipboardcmd}},

    // toggle stuff
    {MODKEY | ShiftMask, XK_b, togglebar, {0}},
    {MODKEY | ShiftMask, XK_space, togglefloating, {0}},
    {MODKEY, XK_f, togglefullscr, {0}},

    {MODKEY | ControlMask, XK_w, tabmode, {-1}},
    {MODKEY, XK_j, focusstack, {.i = +1}},
    {MODKEY, XK_k, focusstack, {.i = -1}},
    {MODKEY, XK_i, incnmaster, {.i = +1}},
    {MODKEY, XK_d, incnmaster, {.i = -1}},

    // shift view
    {MODKEY, XK_Left, shiftview, {.i = -1}},
    {MODKEY, XK_Right, shiftview, {.i = +1}},

    // change m,cfact sizes
    {MODKEY, XK_h, setmfact, {.f = -0.05}},
    {MODKEY, XK_l, setmfact, {.f = +0.05}},
    {MODKEY | ShiftMask, XK_h, setcfact, {.f = +0.25}},
    {MODKEY | ShiftMask, XK_l, setcfact, {.f = -0.25}},
    {MODKEY | ShiftMask, XK_o, setcfact, {.f = 0.00}},

    {MODKEY | ShiftMask, XK_j, movestack, {.i = +1}},
    {MODKEY | ShiftMask, XK_k, movestack, {.i = -1}},
    {MODKEY | ShiftMask, XK_Return, zoom, {0}},
    {MODKEY, XK_Tab, view, {0}},

    // // inner+outer hori, vert gaps
    {MODKEY | ControlMask, XK_6, incrihgaps, {.i = +1}},
    {MODKEY | ControlMask | ShiftMask, XK_6, incrihgaps, {.i = -1}},
    {MODKEY | ControlMask, XK_7, incrivgaps, {.i = +1}},
    {MODKEY | ControlMask | ShiftMask, XK_7, incrivgaps, {.i = -1}},
    {MODKEY | ControlMask, XK_8, incrohgaps, {.i = +1}},
    {MODKEY | ControlMask | ShiftMask, XK_8, incrohgaps, {.i = -1}},
    {MODKEY | ControlMask, XK_9, incrovgaps, {.i = +1}},
    {MODKEY | ControlMask | ShiftMask, XK_9, incrovgaps, {.i = -1}},
    {MODKEY | ControlMask | ShiftMask, XK_d, defaultgaps, {0}},

    // // change border size
    {MODKEY | ShiftMask, XK_minus, setborderpx, {.i = -1}},
    {MODKEY | ShiftMask, XK_p, setborderpx, {.i = +1}},
    {MODKEY | ShiftMask, XK_w, setborderpx, {.i = default_border}},

    // kill dwm
    {MODKEY | ShiftMask, XK_q, spawn, SHCMD("killall bar.sh chadwm")},
    {MODKEY | ShiftMask, XK_m, spawn, SHCMD("rofi-power-menu.sh")},

    // kill window
    {MODKEY, XK_q, killclient, {0}},

    // restart
    {MODKEY | ShiftMask, XK_r, restart, {0}},

    // hide & restore windows
    {MODKEY | ALTKEY, XK_e, hidewin, {0}},
    {MODKEY | ShiftMask, XK_e, restorewin, {0}},

    TAGKEYS(XK_1, 0) TAGKEYS(XK_2, 1) TAGKEYS(XK_3, 2) TAGKEYS(XK_4, 3)
        TAGKEYS(XK_5, 4) TAGKEYS(XK_6, 5) TAGKEYS(XK_7, 6) TAGKEYS(XK_8, 7)
            TAGKEYS(XK_9, 8)};

static const Button buttons[] = {
    /* click                event mask      button          function argument */
    {ClkLtSymbol, 0, Button1, setlayout, {0}},
    {ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]}},
    {ClkWinTitle, 0, Button2, zoom, {0}},
    {ClkStatusText, 0, Button2, spawn, SHCMD("kitty")},

    {ClkClientWin, MODKEY, Button1, moveorplace, {.i = 0}},
    {ClkClientWin, MODKEY, Button2, togglefloating, {0}},
    {ClkClientWin, MODKEY, Button3, resizemouse, {0}},
    {ClkClientWin, ControlMask, Button1, dragmfact, {0}},
    {ClkClientWin, ControlMask, Button3, dragcfact, {0}},
    {ClkTagBar, 0, Button1, view, {0}},
    {ClkTagBar, 0, Button3, toggleview, {0}},
    {ClkTagBar, MODKEY, Button1, tag, {0}},
    {ClkTagBar, MODKEY, Button3, toggletag, {0}},
    {ClkTabBar, 0, Button1, focuswin, {0}},
    {ClkTabBar, 0, Button1, focuswin, {0}},
    {ClkTabPrev, 0, Button1, movestack, {.i = -1}},
    {ClkTabNext, 0, Button1, movestack, {.i = +1}},
    {ClkTabClose, 0, Button1, killclient, {0}},
};
