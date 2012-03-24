/******************************************************************************
 * Copyright (C) 2012 Salvatore Sanfilippo.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <ctype.h>

#include "framebuffer.h"
#include "editor.h"
#include "load81.h"
#include "schemedefs.h"

#define NOTUSED(V) ((void) V)

struct globalConfig l81;

// for installing bindings
struct binding {
  pointer (*func)(scheme *, pointer);
  char *name;
};


/* =========================== Utility functions ============================ */

/* Return the UNIX time in microseconds */
long long ustime(void) {
  struct timeval tv;
  long long ust;

  gettimeofday(&tv, NULL);
  ust = ((long long)tv.tv_sec)*1000000;
  ust += tv.tv_usec;
  return ust;
}

/* Return the UNIX time in milliseconds */
long long mstime(void) {
  return ustime()/1000;
}

/* ========================= Lua helper functions ========================== */


/* Set the error string and the error line number. */
void programError(const char *e) {
  int line = 0;
  char *p;

  if ((p = strchr(e,':')) != NULL)
    line = atoi(p+1)-1;
  editorSetError(e,line);
  l81.scmerr = 1;
}

/* Set a Scheme global to the specified number. */
void setNumber(char *name, int n) {
  SCM_DEFINE(SCM_SYM(name), SCM_INT(n));
}

/* Get a Scheme global containing a number. */
pointer getNumber(char *name) {
  pointer n;
  pointer sym = SCM_SYM(name);

  n = scheme_eval(l81.SC, sym);
  if (!SCM_ISNUM(n)) {
    programError("expected number");
    return l81.SC->NIL;
  }
  return n;
}

/* Set a Lua global table field to the value on the top of the Lua stack. */
void setTableField(char *name, char *field) {
  //    lua_getglobal(l81.L,name);          /* Stack: val table */
  //  /* Create the table if needed */
  //  if (lua_isnil(l81.L,-1)) {
  //    lua_pop(l81.L,1);               /* Stack: val */
  //    lua_newtable(l81.L);            /* Stack: val table */
  //    lua_setglobal(l81.L,name);      /* Stack: val */
  //    lua_getglobal(l81.L,name);      /* Stack: val table */
  //  }
  //  /* Set the field */
  //  if (lua_istable(l81.L,-1)) {
  //    lua_pushstring(l81.L,field);    /* Stack: val table field */
  //    lua_pushvalue(l81.L,-3);        /* Stack: val table field val */
  //    lua_settable(l81.L,-3);         /* Stack: val table */
  //  }
  //  lua_pop(l81.L,2);                   /* Stack: (empty) */*/
}

void setTableFieldNumber(char *name, char *field, pointer n) {
  //  lua_pushnumber(l81.L,n);
  //setTableField(name,field);
}

void setTableFieldString(char *name, char *field, char *s) {
  //lua_pushstring(l81.L,s);
  //setTableField(name,field);
}

/* ============================= Scheme bindings ============================== */
pointer fillBinding(scheme *sc, pointer args) {
  pointer _tmpp;
  double tmp;
  
  if (SCM_LLEN(args) != 4) {
    programError("arity error: fill requires 4 arguments");
    return sc->NIL;
  }

  SCM_POPINT(l81.r, args, "type error: first argument to fill must be numeric", 1);
  SCM_POPINT(l81.g, args, "type error: second argument to fill must be numeric", 1);
  SCM_POPINT(l81.b, args, "type error: third argument to fill must be numeric", 1);
  SCM_POPREAL(tmp, args, "type error: fourth argument to fill must be numeric", 0);

  l81.alpha = (int)(tmp * 255);

  if (l81.r < 0) l81.r = 0;
  if (l81.r > 255) l81.r = 255;
  if (l81.g < 0) l81.g = 0;
  if (l81.g > 255) l81.g = 255;
  if (l81.b < 0) l81.b = 0;
  if (l81.b > 255) l81.b = 255;
  if (l81.alpha < 0) l81.alpha = 0;
  if (l81.alpha > 255) l81.alpha = 255;
  return l81.SC->NIL;
}

pointer rectBinding(scheme *sc, pointer args) {
  int x,y,w,h;

  pointer _tmpp;

  if (SCM_LLEN(args) != 4) {
    programError("arity error: rect requires 4 arguments");
    return sc->NIL;
  }

  SCM_POPINT(x, args, "type error: first argument to rect must be numeric", 1);
  SCM_POPINT(y, args, "type error: second argument to rect must be numeric", 1);
  SCM_POPINT(w, args, "type error: third argument to rect must be numeric", 1);
  SCM_POPINT(h, args, "type error: fourth argument to rect must be numeric", 0);

  drawBox(l81.fb,x,y,x+(w-1),y+(h-1),l81.r,l81.g,l81.b,l81.alpha);
  return sc->NIL;
}

pointer ellipseBinding(scheme *sc, pointer args) {
  int x,y,rx,ry;

  pointer _tmpp;

  if (SCM_LLEN(args) != 4) {
    programError("arity error: ellipse requires 4 arguments");
    return sc->NIL;
  }

  SCM_POPINT(x, args, "type error: first argument to ellipse must be numeric", 1);
  SCM_POPINT(y, args, "type error: second argument to ellipse must be numeric", 1);
  SCM_POPINT(rx, args, "type error: third argument to ellipse must be numeric", 1);
  SCM_POPINT(ry, args, "type error: fourth argument to ellipse must be numeric", 0);

  drawEllipse(l81.fb,x,y,rx,ry,l81.r,l81.g,l81.b,l81.alpha);
  return sc->NIL;
}

pointer triangleBinding(scheme *sc, pointer args) {
  int x1,y1,x2,y2,x3,y3;
  pointer _tmpp;

  if (SCM_LLEN(args) != 6) {
    programError("arity error: triangle requires 6 arguments");
    return sc->NIL;
  }

  SCM_POPINT(x1, args, "type error: first argument to triangle must be numeric", 1);
  SCM_POPINT(y1, args, "type error: second argument to triangle must be numeric", 1);
  SCM_POPINT(x2, args, "type error: third argument to triangle must be numeric", 1);
  SCM_POPINT(y2, args, "type error: fourth argument to triangle must be numeric", 1);

  SCM_POPINT(x3, args, "type error: fifth argument to triangle must be numeric", 1);
  SCM_POPINT(y3, args, "type error: sixth argument to triangle must be numeric", 0);

  drawTriangle(l81.fb,x1,y1,x2,y2,x3,y3,l81.r,l81.g,l81.b,l81.alpha);
  return sc->NIL;
}

pointer lineBinding(scheme *sc, pointer args) {
  int x1,y1,x2,y2;

  pointer _tmpp;

  if (SCM_LLEN(args) != 4) {
    programError("arity error: line requires 4 arguments");
    return sc->NIL;
  }

  SCM_POPINT(x1, args, "type error: first argument to line must be numeric", 1);
  SCM_POPINT(y1, args, "type error: second argument to line must be numeric", 1);
  SCM_POPINT(x2, args, "type error: third argument to line must be numeric", 1);
  SCM_POPINT(y2, args, "type error: fourth argument to line must be numeric", 0);

  drawLine(l81.fb,x1,y1,x2,y2,l81.r,l81.g,l81.b,l81.alpha);
  return sc->NIL;
}

pointer textBinding(scheme *sc, pointer args) {
  int x,y;
  const char *s;
  size_t len;
  pointer tmp;

  pointer _tmpp;

  if (SCM_LLEN(args) != 3) {
    programError("arity error: text requires 4 arguments");
    return sc->NIL;
  }

  SCM_POPINT(x, args, "type error: first argument to line must be numeric", 1);
  SCM_POPINT(y, args, "type error: second argument to line must be numeric", 1);
  SCM_POPSTR(tmp, args, "type error: third argument to line must be a string", 0);

  len = SCM_STRLENGTH(tmp);
  s = SCM_STRVALUE(tmp);

  if (!s) return 0;
  bfWriteString(l81.fb,x,y,s,len,l81.r,l81.g,l81.b,l81.alpha);
  return sc->NIL;
}

pointer setFPSBinding(scheme *sc, pointer args) {
  pointer p;
  if (SCM_LLEN(args) != 1) {
    programError("arity error: set-fps requires one numeric argument");
  }

  p = (SCM_CAR(args));
  if (SCM_ISNUM(p)) {
    l81.fps = (int)SCM_IVAL(p);
    if (l81.fps <= 0) l81.fps = 1;
    SDL_setFramerate(&l81.fb->fps_mgr,l81.fps);
  }
  else {
    programError("framerate must be a numeric");
  }

  return sc->NIL;
}

pointer backgroundBinding(scheme *sc, pointer args) {
  int r,g,b;

  pointer _tmpp;

  if (SCM_LLEN(args) != 3) {
    programError("arity error: text requires 4 arguments");
    return sc->NIL;
  }

  SCM_POPINT(r, args, "type error: first argument to background must be numeric", 1);
  SCM_POPINT(g, args, "type error: second argument to background must be numeric", 1);
  SCM_POPINT(b, args, "type error: third argument to background must be numeric", 0);

  fillBackground(l81.fb,r,g,b);
  return sc->NIL;
}

pointer getpixelBinding(scheme *sc, pointer args) {
  Uint32 pixel;
  Uint8 r, g, b;
  int x, y;

  pointer tmp;

  pointer _tmpp;

  if (SCM_LLEN(args) != 3) {
    programError("arity error: getpixel requires 4 arguments");
    return sc->NIL;
  }

  SCM_POPINT(x, args, "type error: first argument to getpixel must be numeric", 1);
  SCM_POPINT(y, args, "type error: second argument to getpixel must be numeric", 0);

  y = l81.fb->height - 1 - y;

  SDL_LockSurface(l81.fb->screen);
  if (x < 0 || x >= l81.fb->width || y < 0 || y >= l81.fb->height) {
    pixel = 0;
  } else {
    int bpp;
    unsigned char *p;

    bpp = l81.fb->screen->format->BytesPerPixel;
    p = ((unsigned char*) l81.fb->screen->pixels)+
      (y*l81.fb->screen->pitch)+(x*bpp);
    switch(bpp) {
    case 1: pixel = *p; break;
    case 2: pixel = *(Uint16 *)p; break;
    case 3:
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
      pixel = p[0]|p[1]<<8|p[2]<<16;
#else
      pixel = p[2]|p[1]<<8|p[0]<<16;
#endif
    case 4: pixel = *(Uint32*)p; break;
    default: return 0; break;
    }
  }
  SDL_GetRGB(pixel,l81.fb->screen->format,&r,&g,&b);
  SDL_UnlockSurface(l81.fb->screen);
  /* Return the pixel as three values: r, g, b. */

#ifdef cons
#undef cons
#endif
  tmp = SCM_CONS(SCM_INT(r),
                 SCM_CONS(SCM_INT(g),
                          SCM_CONS(SCM_INT(b),
                                   l81.SC->NIL)));

  return tmp;
}

/*pointer spriteBinding(scheme *sc, pointer args) {
  const char *filename;
  int x, y, angle, antialiasing;
  void *sprite;

  filename = lua_tostring(L, 1);
  x = lua_tonumber(L, 2);
  y = lua_tonumber(L, 3);
  angle = luaL_optnumber(L,4,0);
  antialiasing = lua_toboolean(L,5);
  sprite = spriteLoad(L,filename);
  spriteBlit(l81.fb, sprite, x, y, angle, antialiasing);
  return 1;
  }*/

/* ========================== Events processing ============================= */

void setup(void) {
  // obvs need some error checking here
  scheme_apply0(l81.SC, "setup");
}

void draw(void) {
  // obvs need some error checking here
  scheme_apply0(l81.SC, "draw");
}

/* Update the keyboard.pressed and mouse.pressed Lua table. */
void updatePressedState(char *object, char *keyname, int pressed) {
  //  lua_getglobal(l81.L,object);         /* $keyboard */
  //lua_pushstring(l81.L,"pressed");     /* $keyboard, "pressed" */
  //lua_gettable(l81.L,-2);              /* $keyboard, $pressed */
  //lua_pushstring(l81.L,keyname);       /* $keyboard, $pressed, "keyname" */
  //if (pressed) {
  //  lua_pushboolean(l81.L,1);        /* $k, $pressed, "keyname", true */
  //} else {
  //  lua_pushnil(l81.L);              /* $k, $pressed, "keyname", nil */
  //}
  //lua_settable(l81.L,-3);              /* $k, $pressed */
  //lua_pop(l81.L,2);
}

void keyboardEvent(SDL_KeyboardEvent *key, int down) {
  char *keyname = SDL_GetKeyName(key->keysym.sym);

  setTableFieldString("keyboard","state",down ? "down" : "up");
  setTableFieldString("keyboard","key",keyname);
  updatePressedState("keyboard",keyname,down);
}

void mouseMovedEvent(int x, int y, int xrel, int yrel) {
  setNumber("*mouse-x*",x);
  setNumber("*mouse-y*",l81.height-1-y);
  setNumber("*mouse-xrel*",xrel);
  setNumber("*mouse-yrel*",-yrel);
}

void mouseButtonEvent(int button, int pressed) {
  char buttonname[32];
    
  snprintf(buttonname,sizeof(buttonname),"%d",button);
  updatePressedState("mouse",buttonname,pressed);
}

void resetEvents(void) {
  setTableFieldString("keyboard","state","none");
  setTableFieldString("keyboard","key","");
}

void showFPS(void) {
  int elapsed_ms = mstime()-l81.start_ms;
  char buf[64];

  if (!elapsed_ms) return;
  snprintf(buf,sizeof(buf),"FPS: %.2f",(float)(l81.epoch*1000)/elapsed_ms);
  drawBox(l81.fb,0,0,100,20,0,0,0,255);
  bfWriteString(l81.fb,0,0,buf,strlen(buf),128,128,128,255);
}

int processSdlEvents(void) {
  SDL_Event event;

  resetEvents();
  while (SDL_PollEvent(&event)) {
    switch(event.type) {
    case SDL_KEYDOWN:
      switch(event.key.keysym.sym) {
      case SDLK_ESCAPE:
        return 1;
        break;
      default:
        keyboardEvent(&event.key,1);
        break;
      }
      break;
    case SDL_KEYUP:
      keyboardEvent(&event.key,0);
      break;
    case SDL_MOUSEMOTION:
      mouseMovedEvent(event.motion.x,event.motion.y,
                      event.motion.xrel,event.motion.yrel);
      break;
    case SDL_MOUSEBUTTONDOWN:
      mouseButtonEvent(event.button.button,1);
      break;
    case SDL_MOUSEBUTTONUP:
      mouseButtonEvent(event.button.button,0);
      break;
    case SDL_QUIT:
      exit(0);
      break;
    }
    /* If the next event to process is of type KEYUP or
     * MOUSEBUTTONUP we want to stop processing here, so that
     * a fast up/down event be noticed by Lua. */
    if (SDL_PeepEvents(&event,1,SDL_PEEKEVENT,SDL_ALLEVENTS)) {
      if (event.type == SDL_KEYUP ||
          event.type == SDL_MOUSEBUTTONUP)
        break; /* Go to lua before processing more. */
    }
  }

  /* Call the setup function, only the first time. */
  if (l81.epoch == 0) {
    setup();
    if (l81.scmerr) return l81.scmerr;
  }
  /* Call the draw function at every iteration.  */
  draw();
  l81.epoch++;
  /* Refresh the screen */
  if (l81.opt_show_fps) showFPS();
  SDL_Flip(l81.fb->screen);
  /* Wait some time if the frame was produced in less than 1/FPS seconds. */
  SDL_framerateDelay(&l81.fb->fps_mgr);
  /* Stop execution on error */
  return l81.scmerr;
}

/* =========================== Initialization ============================== */

void initConfig(void) {
  l81.width = DEFAULT_WIDTH;
  l81.height = DEFAULT_HEIGHT;
  l81.bpp = DEFAULT_BPP;
  l81.fps = 30;
  l81.r = 255;
  l81.g = l81.b = 0;
  l81.alpha = 255;
  l81.SC = NULL;
  l81.scmerr = 0;
  l81.opt_show_fps = 0;
  l81.opt_full_screen = 0;
  l81.filename = NULL;
  srand(mstime());
}

/* Load the editor program into Lua. Returns 0 on success, 1 on error. */
int loadProgram(void) {
  int buflen;
  char *buf = editorRowsToString(&buflen);

  l81.SC->vptr->load_string(l81.SC, buf);

  l81.scmerr = 0;
  editorClearError();
  return 0;
}

void initScreen(void) {
  l81.fb = createFrameBuffer(l81.width,l81.height,
                             l81.bpp,l81.opt_full_screen);
}


void resetProgram(void) {
  FILE *initfile;
  int i;
  char *initscript = "(begin" \
    "(define *mouse-1-pressed* #f)" \
    "(define *mouse-2-pressed* #f)" \
    "(define *mouse-3-pressed* #f)" \
    "(define *mouse-4-pressed* #f)" \
    "(define *mouse-5-pressed* #f)" \
    "(define *mouse-6-pressed* #f)" \
    "(define *mouse-7-pressed* #f)" \
    "(define *mouse-8-pressed* #f)" \
    "(define *mouse-x* 0)" \
    "(define *mouse-y* 0)" \
    "(define *mouse-xrel* 0)" \
    "(define *mouse-yrel* 0))";

  // TODO: figure out how to deal with keyboard events


  //    "keyboard={}; keyboard['pressed']={};"
  //    "mouse={}; mouse['pressed']={};"
  //    "sprites={}";

  struct binding bindings[9] = {
    { fillBinding, "fill" },
    { rectBinding, "rect" }, 
    { ellipseBinding, "ellipse" }, 
    { backgroundBinding, "background" }, 
    { triangleBinding, "triangle" }, 
    { lineBinding, "line" }, 
    { textBinding, "text" }, 
    { setFPSBinding, "set-fps" }, 
    { getpixelBinding, "getpixel" }
  };

  l81.epoch = 0;
  if (l81.SC) scheme_deinit(l81.SC);
  l81.SC = scheme_init_new();

  // TODO: need to load the scheme init.scm file or else this is useless
  l81.SC->vptr->load_string(l81.SC, initscript);

  setNumber("*width*",l81.width);
  setNumber("*height*",l81.height);

  /*    luaL_loadbuffer(l81.L,initscript,strlen(initscript),"initscript");
        lua_pcall(l81.L,0,0,0);
  */


  /* Make sure that mouse parameters make sense even before the first
   * mouse event captured by SDL */


  for (i = 0; i < 9; i++) {
    l81.SC->vptr->scheme_define(l81.SC,
                                l81.SC->global_env,
                                l81.SC->vptr->mk_symbol(l81.SC, bindings[i].name),
                                bindings[i].func);
  }

  // TODO: sprite engine
  //    initSpriteEngine(l81.L);

  /* Start with a black screen */
  fillBackground(l81.fb,0,0,0);
}

/* ================================= Main ================================== */

void showCliHelp(void) {
  fprintf(stderr, "Usage: load81 [options] program.lua\n"
          "  --width <pixels>       Set screen width\n"
          "  --height <pixels>      Set screen height\n"
          "  --full                 Enable full screen mode\n"
          "  --bpp                  SDL bit per pixel setting (default=24, 0=hardware)\n"
          "  --fps                  Show frames per second\n"
          "  --help                 Show this help screen\n"
          );
  exit(1);
}

void parseOptions(int argc, char **argv) {
  int j;

  for (j = 1; j < argc; j++) {
    char *arg = argv[j];
    int lastarg = j == argc-1;

    if (!strcasecmp(arg,"--fps")) {
      l81.opt_show_fps = 1;
    } else if (!strcasecmp(arg,"--full")) {
      l81.opt_full_screen = 1;
    } else if (!strcasecmp(arg,"--width") && !lastarg) {
      l81.width = atoi(argv[++j]);
    } else if (!strcasecmp(arg,"--height") && !lastarg) {
      l81.height = atoi(argv[++j]);
    } else if (!strcasecmp(arg,"--bpp") && !lastarg) {
      l81.bpp = atoi(argv[++j]);
      if (l81.bpp != 8 && l81.bpp != 16 && l81.bpp != 24 && l81.bpp != 32
          && l81.bpp != 0)
        {
          fprintf(stderr,"Invalid bit per pixel. Try with: 8, 16, 24, 32 or 0 for auto-select the hardware default.");
          exit(1);
        }
    } else if (!strcasecmp(arg,"--help")) {
      showCliHelp();
    } else {
      if (l81.filename == NULL && arg[0] != '-') {
        l81.filename = arg;
      } else {
        fprintf(stderr,
                "Unrecognized option or missing argument: %s\n\n", arg);
        showCliHelp();
      }
    }
  }
  if (l81.filename == NULL) {
    fprintf(stderr,"No scheme program filename specified.\n\n");
    showCliHelp();
  }
}

int main(int argc, char **argv) {
  NOTUSED(argc);
  NOTUSED(argv);

  initConfig();
  parseOptions(argc,argv);
  initScreen();
  initEditor(l81.fb,30,30,30,30);
  editorOpen(l81.filename);
  while(1) {
    resetProgram();
    loadProgram();
    if (!l81.scmerr) {
      SDL_setFramerate(&l81.fb->fps_mgr,l81.fps);
      l81.start_ms = mstime();
      while(!processSdlEvents());
      if (editorFileWasModified()) editorSave(l81.filename);
    }
    editorRun();
  }
  return 0;
}
