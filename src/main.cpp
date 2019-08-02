///////////////////////////////////
/*  Libraries                    */
///////////////////////////////////
#include <fstream>
#include <vector>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <exp_core.h>
#include <exp_sdl.h>
#include "../inc/language.h"

///////////////////////////////////
/*  Joystick codes               */
///////////////////////////////////

#ifdef PLATFORM_GP2X
  #define GP2X_BUTTON_UP              (0)
  #define GP2X_BUTTON_DOWN            (4)
  #define GP2X_BUTTON_LEFT            (2)
  #define GP2X_BUTTON_RIGHT           (6)
  #define GP2X_BUTTON_UPLEFT          (1)
  #define GP2X_BUTTON_UPRIGHT         (7)
  #define GP2X_BUTTON_DOWNLEFT        (3)
  #define GP2X_BUTTON_DOWNRIGHT       (5)
  #define GP2X_BUTTON_CLICK           (18)
  #define GP2X_BUTTON_A               (12)
  #define GP2X_BUTTON_B               (13)
  #define GP2X_BUTTON_X               (14)
  #define GP2X_BUTTON_Y               (15)
  #define GP2X_BUTTON_L               (10)
  #define GP2X_BUTTON_R               (11)
  #define GP2X_BUTTON_START           (8)
  #define GP2X_BUTTON_SELECT          (9)
  #define GP2X_BUTTON_VOLUP           (16)
  #define GP2X_BUTTON_VOLDOWN         (17)
#endif // PLATFORM_GP2X

#ifdef PLATFORM_WIN
  #define PC_BUTTON_UP              (1000)
  #define PC_BUTTON_DOWN            (1010)
  #define PC_BUTTON_LEFT            (1020)
  #define PC_BUTTON_RIGHT           (1030)
  #define PC_BUTTON_A               (0)
  #define PC_BUTTON_B               (1)
  #define PC_BUTTON_X               (2)
  #define PC_BUTTON_Y               (3)
  #define PC_BUTTON_L               (4)
  #define PC_BUTTON_R               (5)
  #define PC_BUTTON_START           (7)
  #define PC_BUTTON_BACK            (6)
  #define PC_HAT_UP                 1
  #define PC_HAT_RIGHT              2
  #define PC_HAT_DOWN               4
  #define PC_HAT_LEFT               8
#endif // PLATFORM_WIN

///////////////////////////////////
/*  Program modes                */
///////////////////////////////////
#define PROGRAM_MODE_MENU   1
#define PROGRAM_MODE_GAME   2
#define PROGRAM_MODE_PAUSE  3
#define PROGRAM_MODE_END    4

///////////////////////////////////
/*  Ship directions              */
///////////////////////////////////
#define DIR_DOWN    0
#define DIR_LEFT    1
#define DIR_RIGHT   2

///////////////////////////////////
/*  Structs                      */
///////////////////////////////////
struct gold_box
{
  int x;
  int y;
  int exist;
  int carried;
};

struct cloud_base
{
  int x;
  int y;
  float frame;
  float vel;
};

struct green_base
{
  int x;
  int y;
  float frame;
};

struct bug_base
{
  int x;
  int y;
  int dir_x;
  int dir_y;
};

struct bubble_base
{
  int x;
  int y;
  float ah;
  float av;
};

struct record
{
  char name[21];
  int score;
};

struct joystick_state
{
  int left;
  int right;
  int up;
  int down;
  int pad_left;
  int pad_right;
  int pad_up;
  int pad_down;
  int button_a;
  int button_b;
  int button_x;
  int button_y;
  int button_l;
  int button_r;
  int button_back;
  int button_start;
  int escape;
  int any;
};

///////////////////////////////////
/*  Globals                      */
///////////////////////////////////
SDL_Surface *screen;   		    // screen to work
SDL_Surface *screen2;   		    // real screen of game (it will be x2 zoomed)
int done=0;
int program_mode=PROGRAM_MODE_MENU;
TTF_Font *font;                 // used font
SDL_Joystick *joystick;         // used joystick
joystick_state mainjoystick;
int volume=120;
int scanlines=0;
int fullscreen=0;
#ifdef PLATFORM_WIN
Uint8 *keys=SDL_GetKeyState(NULL);
#endif

language lang;

// graficos
SDL_Surface *ship;
SDL_Surface *shipdisabled;
SDL_Surface *bug;
SDL_Surface *gold;
SDL_Surface *boat;
SDL_Surface *bubble;
SDL_Surface *cloud;
SDL_Surface *green[4];
//sonidos
Mix_Chunk *sound_bubble;
Mix_Chunk *sound_gold;
Mix_Chunk *sound_hit;
Mix_Chunk *sound_roar;
Mix_Chunk *sound_water;
Mix_Chunk *sound_engine;

///////////////////////////////////
/*  Menu variables               */
///////////////////////////////////
int menu_selection=0;

///////////////////////////////////
/*  Game variables               */
///////////////////////////////////
int level=0;
int score=0;
int ship_disabled=false;
int ship_x=0;
int ship_y=0;
float ship_ah=0;
float ship_av=0;
int ship_load=0;
int engine_on=false;
float water_wave=0;
gold_box gold_list[4];
std::vector<bug_base> bug_list;
std::vector<bubble_base> bubble_list;
std::vector<record> record_list;
std::vector<green_base> green_list;
std::vector<cloud_base> cloud_list;

///////////////////////////////////
/*  Exp variables                */
///////////////////////////////////
Uint32 floating_time;
Uint32 floor_time;

///////////////////////////////////
/*  Function declarations        */
///////////////////////////////////
void process_events();
void process_joystick();

///////////////////////////////////
/*  Functions                    */
///////////////////////////////////
void draw_text(SDL_Surface* dst, char* string, int x, int y, int fR, int fG, int fB)
{
  if(dst && string && font)
  {
    SDL_Color foregroundColor={fR,fG,fB};
    SDL_Surface *textSurface=TTF_RenderText_Blended(font,string,foregroundColor);
    if(textSurface)
    {
      SDL_Rect textLocation={x,y,0,0};
      SDL_BlitSurface(textSurface,NULL,dst,&textLocation);
      SDL_FreeSurface(textSurface);
    }
  }
}

///////////////////////////////////
/*  Get pixel from surface       */
///////////////////////////////////
Uint32 get_pixel(SDL_Surface* src, Uint32 x, Uint32 y)
{
  Uint32	color = 0;
	Uint8	*ubuff8;
	Uint16	*ubuff16;
	Uint32	*ubuff32;

	color = 0;

	switch(src->format->BytesPerPixel)
	{
		case 1:
			ubuff8 = (Uint8*) src->pixels;
			ubuff8 += (y * src->pitch) + x;
			color = *ubuff8;
			break;

		case 2:
			ubuff8 = (Uint8*) src->pixels;
			ubuff8 += (y * src->pitch) + (x*2);
			ubuff16 = (Uint16*) ubuff8;
			color = *ubuff16;
			break;

		case 3:
			ubuff8 = (Uint8*) src->pixels;
			ubuff8 += (y * src->pitch) + (x*3);
			color = 0;
			#if SDL_BYTEORDER == SDL_LIL_ENDIAN
				color |= ubuff8[2] << 16;
				color |= ubuff8[1] << 8;
				color |= ubuff8[0];
			#else
				color |= ubuff8[0] << 16;
				color |= ubuff8[1] << 8;
				color |= ubuff8[2];
			#endif
			break;

		case 4:
			ubuff8 = (Uint8*) src->pixels;
			ubuff8 += (y*src->pitch) + (x*4);
			ubuff32 = (Uint32*)ubuff8;
			color = *ubuff32;
			break;

		default:
			break;
	}
	return color;
}

///////////////////////////////////
/*  Draw pixel in surface        */
///////////////////////////////////
void set_pixel(SDL_Surface* src, int x, int y, SDL_Color& color)
{
	Uint8	*ubuff8;
	Uint16	*ubuff16;
	Uint32	*ubuff32;
  Uint32 c=SDL_MapRGB(src->format, color.r, color.g, color.b);

	switch(src->format->BytesPerPixel)
	{
		case 1:
			ubuff8 = (Uint8*) src->pixels;
			ubuff8 += (y * src->pitch) + x;
			*ubuff8 = (Uint8) c;
			break;

		case 2:
			ubuff8 = (Uint8*) src->pixels;
			ubuff8 += (y * src->pitch) + (x*2);
			ubuff16 = (Uint16*) ubuff8;
			*ubuff16 = (Uint16) c;
			break;

		case 3:
			ubuff8 = (Uint8*) src->pixels;
			ubuff8 += (y * src->pitch) + (x*3);
			#if SDL_BYTEORDER == SDL_LIL_ENDIAN
			ubuff8[0] = (Uint8) color.b;
			ubuff8[1] = (Uint8) color.g;
			ubuff8[2] = (Uint8) color.r;
			#else
			ubuff8[0] = (Uint8) color.r;
			ubuff8[1] = (Uint8) color.g;
			ubuff8[2] = (Uint8) color.b;
			#endif
			break;

		case 4:
			ubuff8 = (Uint8*) src->pixels;
			ubuff8 += (y*src->pitch) + (x*4);
			ubuff32 = (Uint32*)ubuff8;
			*ubuff32=(Uint32)c;
			break;

		default:
			break;
	}
}

///////////////////////////////////
/*  Filter surface               */
///////////////////////////////////
void filter_surface(SDL_Surface *src, SDL_Surface *dst)
{
  int climit=40;
  SDL_LockSurface(dst);
  SDL_LockSurface(src);
  for(int g=0; g<240; g++)
  {
    for(int f=0; f<320; f++)
    {
      Uint32 colour=get_pixel(src,f,g);
      SDL_Color c,c2;
      SDL_GetRGB(colour, src->format, &c.r, &c.g, &c.b);
      if(scanlines)
      {
        if(c.r>climit)
          c2.r=c.r-climit;
        else
          c2.r=0;
        if(c.g>climit)
          c2.g=c.g-climit;
        else
          c2.g=0;
        if(c.b>climit)
          c2.b=c.b-climit;
        else
          c2.b=0;
      }
      else
        c2=c;
      set_pixel(dst,f*2,g*2,c);
      set_pixel(dst,f*2+1,g*2,c);
      set_pixel(dst,f*2,g*2+1,c2);
      set_pixel(dst,f*2+1,g*2+1,c2);
    }
  }
  SDL_UnlockSurface(dst);
  SDL_UnlockSurface(src);
}

void clear_joystick_state()
{
  mainjoystick.left=0;
  mainjoystick.right=0;
  mainjoystick.up=0;
  mainjoystick.down=0;
  mainjoystick.pad_left=0;
  mainjoystick.pad_right=0;
  mainjoystick.pad_up=0;
  mainjoystick.pad_down=0;
  mainjoystick.button_a=0;
  mainjoystick.button_b=0;
  mainjoystick.button_x=0;
  mainjoystick.button_y=0;
  mainjoystick.button_l=0;
  mainjoystick.button_r=0;
  mainjoystick.button_back=0;
  mainjoystick.button_start=0;
  mainjoystick.escape=0;
  mainjoystick.any=0;
}

void load_records()
{
  record_list.clear();

  FILE* record_file;
  record_file=fopen("records.dat","rb");
  if(record_file)
  {
    record r;
    while(fread(&r, sizeof(record), 1, record_file))
    {
      record_list.push_back(r);
    }

    fclose(record_file);
  }
}

void save_records()
{
  FILE* record_file;
  record_file=fopen("records.dat","wb");
  if(record_file)
  {
    for(int f=0; f<record_list.size(); f++)
      fwrite(&record_list[f], sizeof(record), 1, record_file);
    fclose(record_file);
  }
}

void init_exp()
{
  if(exp_init("Rafa Vico","Batiscafo")==EXP_READY)
  {
    // enter EXPS (200 points - 8 achivements)
    exp_add(1,10);  // your first treasure
    exp_add(2,20);  // 5 treasures
    exp_add(3,30);  // 10 treasures
    exp_add(4,50);  // 20 treasures
    exp_add(5,10,1);  // nothing
    exp_add(6,20);  // floating 30 seconds
    exp_add(7,20);  // floating 30 seconds with treasure
    exp_add(8,40);  // 1 minute at floor

    // spanish texts
    exp_add_desc(1,"El primero.","Has recuperado tu primer tesoro.","Spanish");
    exp_add_desc(2,"Buzo novato.","Has recuperado 5 tesoros.","Spanish");
    exp_add_desc(3,"Buzo experto.","Has recuperado 10 tesoros.","Spanish");
    exp_add_desc(4,"Buzo maestro.","Has recuperado 20 tesoros.","Spanish");
    exp_add_desc(5,"En blanco.","No has podido recuperar ni un tesoro.","Spanish");
    exp_add_desc(6,"Entre aguas.","Has flotado entre aguas durante 30 segundos.","Spanish");
    exp_add_desc(7,"A la espera.","Has flotado 30 segundos cargado con un tesoro.","Spanish");
    exp_add_desc(8,"Demasiado lastre.","Has aguantado 1 minuto posado en el fondo.","Spanish");

    // english texts
    exp_add_desc(1,"The first.","You recovered your first treasure.","English");
    exp_add_desc(2,"Novice diver.","You recovered 5 treasures.","English");
    exp_add_desc(3,"Expert diver.","You recovered 10 treasures.","English");
    exp_add_desc(4,"Master diver.","You recovered 20 treasures.","English");
    exp_add_desc(5,"Blank.","Don't recovered one treasure ever.","English");
    exp_add_desc(6,"Between waters.","You have floated between waters for 30 seconds.","English");
    exp_add_desc(7,"Waiting.","You've floated 30 seconds loaded with a treasure.","English");
    exp_add_desc(8,"Too much ballast.","You have endured 1 minute perched in the background.","English");

    for(int i=0; i<8; i++)
    {
      char n[20];
      sprintf(n,"data/exp0%i.bmp",i+1);
      exp_add_img(i+1,n);
    }
    exp_add_icon("data/exp.icon.bmp");
  }

  exp_screen(screen);
  exp_set_callback(&exp_callback);

  // set default language from profile
  lang.set_language(0);
  std::string lng=std::string(exp_get_lang());
  for(int f=0; f<lang.languages_count(); f++)
  {
    std::string lng2=std::string(lang.language_name(f));
    if(lng==lng2)
    {
      lang.set_language(f);
      break;
    }
  }
}

void init_game()
{
  srand(time(NULL));
  joystick=SDL_JoystickOpen(0);
  SDL_ShowCursor(0);

  Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, AUDIO_S16, MIX_DEFAULT_CHANNELS, 1024);

  TTF_Init();
  font=TTF_OpenFont("data/pixantiqua.ttf", 12);

  SDL_Rect rect;
  SDL_Surface *tmpsurface;

  tmpsurface=SDL_LoadBMP("data/ship.bmp");
  if(tmpsurface)
  {
    ship=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, tmpsurface->w, tmpsurface->h, 16, 0,0,0,0);
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    SDL_BlitSurface(tmpsurface,&rect,ship,NULL);
    SDL_SetColorKey(ship,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("data/shipdisabled.bmp");
  if(tmpsurface)
  {
    shipdisabled=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, tmpsurface->w, tmpsurface->h, 16, 0,0,0,0);
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    SDL_BlitSurface(tmpsurface,&rect,shipdisabled,NULL);
    SDL_SetColorKey(shipdisabled,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }

  tmpsurface=SDL_LoadBMP("data/bug.bmp");
  if(tmpsurface)
  {
    bug=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, tmpsurface->w, tmpsurface->h, 16, 0,0,0,0);
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    SDL_BlitSurface(tmpsurface,&rect,bug,NULL);
    SDL_SetColorKey(bug,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }
  tmpsurface=SDL_LoadBMP("data/gold.bmp");
  if(tmpsurface)
  {
    gold=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, tmpsurface->w, tmpsurface->h, 16, 0,0,0,0);
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    SDL_BlitSurface(tmpsurface,&rect,gold,NULL);
    SDL_SetColorKey(gold,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }
  tmpsurface=SDL_LoadBMP("data/boat.bmp");
  if(tmpsurface)
  {
    boat=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, tmpsurface->w, tmpsurface->h, 16, 0,0,0,0);
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    SDL_BlitSurface(tmpsurface,&rect,boat,NULL);
    SDL_SetColorKey(boat,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }
  tmpsurface=SDL_LoadBMP("data/bubble.bmp");
  if(tmpsurface)
  {
    bubble=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, tmpsurface->w, tmpsurface->h, 16, 0,0,0,0);
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    SDL_BlitSurface(tmpsurface,&rect,bubble,NULL);
    SDL_SetColorKey(bubble,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }
  tmpsurface=SDL_LoadBMP("data/cloud.bmp");
  if(tmpsurface)
  {
    cloud=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, tmpsurface->w, tmpsurface->h, 16, 0,0,0,0);
    rect.x=0;
    rect.y=0;
    rect.w=tmpsurface->w;
    rect.h=tmpsurface->h;
    SDL_BlitSurface(tmpsurface,&rect,cloud,NULL);
    SDL_SetColorKey(cloud,SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
    SDL_FreeSurface(tmpsurface);
  }
  tmpsurface=SDL_LoadBMP("data/green.bmp");
  if(tmpsurface)
  {
    for(int f=0; f<4; f++)
    {
      rect.x=f*8;
      rect.y=0;
      rect.w=8;
      rect.h=16;
      green[f]=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, 8, 16, 16, 0,0,0,0);
      if(green[f])
      {
        SDL_BlitSurface(tmpsurface,&rect,green[f],NULL);
        SDL_SetColorKey(green[f],SDL_SRCCOLORKEY,SDL_MapRGB(screen->format,255,0,255));
      }
    }
    SDL_FreeSurface(tmpsurface);
  }

  sound_bubble=Mix_LoadWAV("data/bubble.wav");
  sound_gold=Mix_LoadWAV("data/gold.wav");
  sound_hit=Mix_LoadWAV("data/hit.wav");
  sound_roar=Mix_LoadWAV("data/roar.wav");
  sound_water=Mix_LoadWAV("data/water.wav");
  sound_engine=Mix_LoadWAV("data/engine.wav");

  Mix_PlayChannel(-1,sound_water,-1);
}

void end_game()
{
	SDL_FillRect(screen, NULL, 0x000000);

  if(SDL_JoystickOpened(0))
    SDL_JoystickClose(joystick);

  if(ship)
    SDL_FreeSurface(ship);
  if(shipdisabled)
    SDL_FreeSurface(ship);
  if(bug)
    SDL_FreeSurface(bug);
  if(gold)
    SDL_FreeSurface(gold);
  if(boat)
    SDL_FreeSurface(boat);
  if(bubble)
    SDL_FreeSurface(bubble);
  if(cloud)
    SDL_FreeSurface(cloud);
  for(int f=0; f<4; f++)
    if(green[f])
      SDL_FreeSurface(green[f]);

  Mix_HaltChannel(-1);
  Mix_FreeChunk(sound_bubble);
  Mix_FreeChunk(sound_gold);
  Mix_FreeChunk(sound_hit);
  Mix_FreeChunk(sound_roar);
  Mix_FreeChunk(sound_water);
  Mix_FreeChunk(sound_engine);
  Mix_CloseAudio();
}

void new_bubble(int x, int y, int dir)
{
  bubble_base b;
  b.x=x-4+rand()%9;
  b.y=y-4+rand()%9;
  switch(dir)
  {
    case DIR_DOWN:
      b.av=rand()%5;
      b.ah=rand()%2;
      break;
    case DIR_LEFT:
      b.av=rand()%2;
      b.ah=-rand()%5;
      break;
    case DIR_RIGHT:
      b.av=rand()%2;
      b.ah=rand()%5;
      break;
  }

  bubble_list.push_back(b);
  if(rand()%6==0)
    Mix_PlayChannel(-1,sound_bubble,0);
}

void new_bug()
{
  bug_base b;
  b.x=rand()%298;
  b.y=72+rand()%132;
  b.dir_x=0;
  b.dir_y=0;
  while(b.dir_x==0 && b.dir_y==0)
  {
    b.dir_x=-1+rand()%3;
    b.dir_y=-1+rand()%3;
  }
  if(b.x>130 && b.x<190 && b.y<100 && b.dir_y<0)
    b.dir_y=-b.dir_y;

  bug_list.push_back(b);
}

void new_level()
{
  // create treasures
  for(int i=0; i<4; i++)
  {
    gold_list[i].x=28+80*i;
    gold_list[i].y=228;
    gold_list[i].exist=1;
    gold_list[i].carried=0;
  }
  // add a new bug
  new_bug();
  // inc level
  level++;
}

void reset()
{
  level=0;
  score=0;
  ship_x=148;
  ship_y=48;
  ship_ah=0;
  ship_av=0;
  ship_load=0;

  bug_list.clear();
  new_bug();
  new_bug();
  bubble_list.clear();
  green_list.clear();
  cloud_list.clear();

  floating_time=SDL_GetTicks();
  floor_time=SDL_GetTicks();

  // plants
  for(int f=0;;)
  {
    green_base g;
    g.y=212;
    g.x=f;
    f+=6+rand()%8;
    g.frame=rand()%4;
    green_list.push_back(g);
    if(f>312)
      break;
  }
  // clouds
  for(int f=0;f<5;f++)
  {
    cloud_base c;
    c.y=-16+rand()%16;
    c.x=rand()%320;
    if(rand()%2==0)
      c.frame=1;
    else
      c.frame=-1;
    c.vel=0.1+float(rand()%10)/10;
    cloud_list.push_back(c);
  }
}

void check_score()
{
  record r;
  if(exp_user())
    sprintf(r.name,exp_user());
  else
    sprintf(r.name,"player");
  r.score=score;

  int i;
  for(i=0; i<record_list.size(); i++)
    if(record_list[i].score<=score)
      break;

  if(i>=record_list.size() && record_list.size()<5)
    record_list.push_back(r);
  else
  {
    record_list.insert(record_list.begin()+i,r);
    if(record_list.size()>5)
      record_list.pop_back();
  }
}

void finish()
{
  check_score();
  Mix_HaltChannel(4);
  Mix_PlayChannel(-1,sound_roar,0);
  if(score==0)
    exp_win(5);
  program_mode=PROGRAM_MODE_END;
}

void read_menu_keys()
{
  process_events();

  if(mainjoystick.pad_up)
    if(menu_selection>0)
        menu_selection--;
  if(mainjoystick.pad_down)
    if(menu_selection<2)
        menu_selection++;
  if(mainjoystick.button_a)
    switch(menu_selection)
    {
      case 0:
        reset();
        new_level();
        ship_disabled=true;
        program_mode=PROGRAM_MODE_GAME;
        break;
      case 1:
        int lid=lang.language_id()+1;
        if(lid>lang.languages_count()-1)
          lid=0;
        lang.set_language(lid);
        break;
      case 2:
        done=1;
        break;
    }
}

void process_events()
{
  SDL_Event event;

  clear_joystick_state();
  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
#ifdef PLATFORM_GP2X
      case SDL_JOYBUTTONDOWN:
        switch (event.jbutton.button)
        {
          case GP2X_BUTTON_LEFT:
            mainjoystick.pad_left=1;
            break;
          case GP2X_BUTTON_RIGHT:
            mainjoystick.pad_right=1;
            break;
          case GP2X_BUTTON_UP:
            mainjoystick.pad_up=1;
            break;
          case GP2X_BUTTON_DOWN:
            mainjoystick.pad_down=1;
            break;
          case GP2X_BUTTON_Y:
            mainjoystick.button_y=1;
            break;
          case GP2X_BUTTON_X:
            mainjoystick.button_a=1;
            break;
          case GP2X_BUTTON_B:
            mainjoystick.button_b=1;
            break;
          case GP2X_BUTTON_A:
            mainjoystick.button_x=1;
            break;
          case GP2X_BUTTON_START:
            mainjoystick.button_start=1;
            break;
          case GP2X_BUTTON_SELECT:
            mainjoystick.button_back=1;
            break;
        }
        mainjoystick.any=1;
        break;
#endif // PLATFORM_GP2X
#ifdef PLATFORM_WIN
      case SDL_KEYDOWN:
        switch(event.key.keysym.sym)
        {
          case SDLK_LEFT:
            mainjoystick.pad_left=1;
            break;
          case SDLK_RIGHT:
            mainjoystick.pad_right=1;
            break;
          case SDLK_UP:
            mainjoystick.pad_up=1;
            break;
          case SDLK_DOWN:
            mainjoystick.pad_down=1;
            break;
          case SDLK_a:
            mainjoystick.button_x=1;
            break;
          case SDLK_s:
            mainjoystick.button_y=1;
            break;
          case SDLK_RETURN:
          case SDLK_z:
            mainjoystick.button_a=1;
            break;
          case SDLK_x:
            mainjoystick.button_b=1;
            break;
          case SDLK_ESCAPE:
            mainjoystick.escape=1;
            break;
        }
        mainjoystick.any=1;
        break;
      case SDL_JOYBUTTONDOWN:
        switch (event.jbutton.button)
        {
          case PC_BUTTON_X:
            mainjoystick.button_x=1;
            break;
          case PC_BUTTON_Y:
            mainjoystick.button_y=1;
            break;
          case PC_BUTTON_A:
            mainjoystick.button_a=1;
            break;
          case PC_BUTTON_B:
            mainjoystick.button_b=1;
            break;
          case PC_BUTTON_BACK:
            mainjoystick.button_back=1;
            break;
          case PC_BUTTON_START:
            mainjoystick.button_start=1;
            break;
        }
        mainjoystick.any=1;
        break;
      case SDL_JOYAXISMOTION:
        switch(event.jaxis.axis)
        {
          case 0:
            if(event.jaxis.value<0)
            {
              mainjoystick.left=event.jaxis.value;
              mainjoystick.right=0;
              if(event.jaxis.value<-32000)
              {
                mainjoystick.pad_left=1;
                mainjoystick.any=1;
              }
            }
            else
            {
              mainjoystick.right=event.jaxis.value;
              mainjoystick.left=0;
              if(event.jaxis.value>32000)
              {
                mainjoystick.pad_right=1;
                mainjoystick.any=1;
              }
            }
            break;
          case 1:
            if(event.jaxis.value<0)
            {
              mainjoystick.up=event.jaxis.value;
              mainjoystick.down=0;
              if(event.jaxis.value<-32000)
              {
                mainjoystick.pad_up=1;
                mainjoystick.any=1;
              }
            }
            else
            {
              mainjoystick.down=event.jaxis.value;
              mainjoystick.up=0;
              if(event.jaxis.value>32000)
              {
                mainjoystick.pad_down=1;
                mainjoystick.any=1;
              }
            }
            break;
        }
        break;
      case SDL_JOYHATMOTION:
        switch(event.jhat.value)
        {
          case 1:
            mainjoystick.pad_up=1;
            break;
          case 2:
            mainjoystick.pad_right=1;
            break;
          case 4:
            mainjoystick.pad_down=1;
            break;
          case 8:
            mainjoystick.pad_left=1;
            break;
        }
        mainjoystick.any=1;
        break;
#endif // PLATFORM_WIN
      }
  }
}

// process keyboard and joystick (no events), and save in mainjoystick variable
void process_joystick()
{
#ifdef PLATFORM_GP2X
  if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_START))
    mainjoystick.button_start=1;
  else
    mainjoystick.button_start=0;
  if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_SELECT))
    mainjoystick.button_back=1;
  else
    mainjoystick.button_back=0;
  if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_LEFT) || SDL_JoystickGetButton(joystick, GP2X_BUTTON_UPLEFT) || SDL_JoystickGetButton(joystick, GP2X_BUTTON_DOWNLEFT))
    mainjoystick.pad_left=1;
  else
    mainjoystick.pad_left=0;
  if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_RIGHT) || SDL_JoystickGetButton(joystick, GP2X_BUTTON_UPRIGHT) || SDL_JoystickGetButton(joystick, GP2X_BUTTON_DOWNRIGHT))
    mainjoystick.pad_right=1;
  else
    mainjoystick.pad_right=0;
  if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_UP) || SDL_JoystickGetButton(joystick, GP2X_BUTTON_UPLEFT) || SDL_JoystickGetButton(joystick, GP2X_BUTTON_UPRIGHT))
    mainjoystick.pad_up=1;
  else
    mainjoystick.pad_up=0;
  if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_DOWN) || SDL_JoystickGetButton(joystick, GP2X_BUTTON_DOWNLEFT) || SDL_JoystickGetButton(joystick, GP2X_BUTTON_DOWNRIGHT))
    mainjoystick.pad_down=1;
  else
    mainjoystick.pad_down=0;
  if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_X))
    mainjoystick.button_a=1;
  else
    mainjoystick.button_a=0;
  if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_B))
    mainjoystick.button_b=1;
  else
    mainjoystick.button_b=0;
  if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_A))
    mainjoystick.button_x=1;
  else
    mainjoystick.button_x=0;
  if(SDL_JoystickGetButton(joystick, GP2X_BUTTON_Y))
    mainjoystick.button_y=1;
  else
    mainjoystick.button_y=0;
#endif // PLATFORM_GP2X
#ifdef PLATFORM_WIN
  if(keys[SDLK_ESCAPE])
    mainjoystick.escape=1;
  else
    mainjoystick.escape=0;
  if(keys[SDLK_z] || SDL_JoystickGetButton(joystick, PC_BUTTON_A))
    mainjoystick.button_a=1;
  else
    mainjoystick.button_a=0;
  if(keys[SDLK_x] || SDL_JoystickGetButton(joystick, PC_BUTTON_B))
    mainjoystick.button_b=1;
  else
    mainjoystick.button_b=0;
  if(keys[SDLK_a] || SDL_JoystickGetButton(joystick, PC_BUTTON_X))
    mainjoystick.button_x=1;
  else
    mainjoystick.button_x=0;
  if(keys[SDLK_s] || SDL_JoystickGetButton(joystick, PC_BUTTON_Y))
    mainjoystick.button_y=1;
  else
    mainjoystick.button_y=0;

  if(keys[SDLK_LEFT] || SDL_JoystickGetHat(joystick,0)&SDL_HAT_LEFT || (SDL_JoystickGetAxis(joystick,0)<-15000))// || SDL_JoystickGetHat(joystick,0)&SDL_HAT_LEFTUP || SDL_JoystickGetHat(joystick,0)&SDL_HAT_LEFTDOWN)
    mainjoystick.pad_left=1;
  else
    mainjoystick.pad_left=0;
  if(keys[SDLK_RIGHT] || SDL_JoystickGetHat(joystick,0)&SDL_HAT_RIGHT || (SDL_JoystickGetAxis(joystick,0)>15000))// || SDL_JoystickGetHat(joystick,0)&SDL_HAT_RIGHTUP || SDL_JoystickGetHat(joystick,0)&SDL_HAT_RIGHTDOWN)
    mainjoystick.pad_right=1;
  else
    mainjoystick.pad_right=0;
  if(keys[SDLK_UP] || SDL_JoystickGetHat(joystick,0)&SDL_HAT_UP || (SDL_JoystickGetAxis(joystick,1)<-15000))// || SDL_JoystickGetHat(joystick,0)&SDL_HAT_LEFTUP || SDL_JoystickGetHat(joystick,0)&SDL_HAT_RIGHTUP)
    mainjoystick.pad_up=1;
  else
    mainjoystick.pad_up=0;
  if(keys[SDLK_DOWN] || SDL_JoystickGetHat(joystick,0)&SDL_HAT_DOWN || (SDL_JoystickGetAxis(joystick,1)>15000))// || SDL_JoystickGetHat(joystick,0)&SDL_HAT_LEFTDOWN || SDL_JoystickGetHat(joystick,0)&SDL_HAT_RIGHTDOWN)
    mainjoystick.pad_down=1;
  else
    mainjoystick.pad_down=0;
  if(SDL_JoystickGetButton(joystick, PC_BUTTON_START))
    mainjoystick.button_start=1;
  else
    mainjoystick.button_start=0;
  if(SDL_JoystickGetButton(joystick, PC_BUTTON_BACK))
    mainjoystick.button_back=1;
  else
    mainjoystick.button_back=0;
  if(SDL_JoystickGetButton(joystick, PC_BUTTON_L))
    mainjoystick.button_l=1;
  else
    mainjoystick.button_l=0;
  if(SDL_JoystickGetButton(joystick, PC_BUTTON_R))
    mainjoystick.button_r=1;
  else
    mainjoystick.button_r=0;
#endif // PLATFORM_WIN
}

void draw_menu()
{
  SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,56,152,255));

  SDL_Rect rbubble;
  for(int i=0; i<bubble_list.size(); i++)
  {
    rbubble.x=bubble_list[i].x;
    rbubble.y=bubble_list[i].y;
    if(bubble)
      SDL_BlitSurface(bubble,NULL,screen,&rbubble);
  }

  draw_text(screen,lang.get_string(1),50,50,255,255,255);
  draw_text(screen,lang.get_string(6),50,64,255,255,255);

  draw_text(screen,lang.get_string(2),50,100,255,255,255);
  draw_text(screen,lang.language_name(lang.language_id()),50,120,255,255,255);
  draw_text(screen,lang.get_string(3),50,140,255,255,255);

  switch(menu_selection)
  {
    case 0:
      draw_text(screen,lang.get_string(2),49,99,255,0,0);
      break;
    case 1:
      draw_text(screen,lang.language_name(lang.language_id()),49,119,255,0,0);
      break;
    case 2:
      draw_text(screen,lang.get_string(3),49,139,255,0,0);
      break;
  }

  for(int i=0; i<record_list.size(); i++)
  {
    char recordline[50];
    sprintf(recordline,"%i - %s",record_list[i].score, record_list[i].name);
    draw_text(screen,recordline,200,50+i*15,192,192,192);
  }
}

void update_menu()
{
  // draw menu

  if(rand()%5==0)
    new_bubble(8+rand()%304,230,1+rand()%2);
  // move bubbles
  for(int i=0; i<bubble_list.size(); i++)
  {
    bubble_list[i].x+=(int)bubble_list[i].ah-1+rand()%3;
    bubble_list[i].y+=(int)bubble_list[i].av;
    if(bubble_list[i].ah<0)
      bubble_list[i].ah+=0.3;
    if(bubble_list[i].ah>0)
      bubble_list[i].ah-=0.3;
    if(bubble_list[i].av>-1)
      bubble_list[i].av-=0.3;
    if(bubble_list[i].y<0)
    {
      bubble_list.erase(bubble_list.begin()+i);
      i--;
    }
    else if(rand()%300==0)
    {
      bubble_list.erase(bubble_list.begin()+i);
      i--;
    }
  }

  read_menu_keys();
  draw_menu();
}

void read_game_keys()
{
  process_events();

  if(mainjoystick.any && ship_disabled)
    ship_disabled=false;
  if(mainjoystick.button_start)
    program_mode=PROGRAM_MODE_MENU;
  if(mainjoystick.button_back || mainjoystick.escape)
    program_mode=PROGRAM_MODE_PAUSE;

  process_joystick();

  if(mainjoystick.button_a)
  {
    if(!ship_disabled)
    {
      if(ship_av>-4)
        ship_av-=0.1;
      if(!engine_on)
      {
        Mix_PlayChannel(4,sound_engine,-1);
        engine_on=true;
      }
    }
    if(rand()%3==0)
      new_bubble(ship_x+12,ship_y+22,DIR_DOWN);
  }
  else
  {
    if(!ship_disabled)
      if(ship_av<4)
        ship_av+=0.1;
    if(engine_on)
    {
      Mix_HaltChannel(4);
      engine_on=false;
    }
  }

  if(mainjoystick.pad_left)
  {
    if(!ship_disabled)
      if(ship_ah>-4)
        ship_ah-=0.1;
    if(rand()%3==0)
      new_bubble(ship_x+22,ship_y+7,DIR_RIGHT);
  }
  if(mainjoystick.pad_right)
  {
    if(!ship_disabled)
      if(ship_ah<4)
        ship_ah+=0.1;
    if(rand()%3==0)
      new_bubble(ship_x+2,ship_y+7,DIR_LEFT);
  }

  if(!mainjoystick.pad_left && ship_ah<0 && !ship_disabled)
    ship_ah+=0.1;

  if(!mainjoystick.pad_right && ship_ah>0 && !ship_disabled)
    ship_ah-=0.1;
}

void draw_game()
{
  // draw layers
  SDL_Rect dest;  // sky
  dest.x=0;
  dest.y=0;
  dest.w=320;
  dest.h=48;
  SDL_FillRect(screen,&dest,SDL_MapRGB(screen->format,110,238,255));
  dest.x=0;       // sea
  dest.y=48;
  dest.w=320;
  dest.h=179;
  SDL_FillRect(screen,&dest,SDL_MapRGB(screen->format,56,152,255));
  dest.x=0;       // floor
  dest.y=227;
  dest.w=320;
  dest.h=13;
  SDL_FillRect(screen,&dest,SDL_MapRGB(screen->format,207,121,87));
  dest.x=0;       // plants
  dest.y=227;
  dest.w=320;
  dest.h=1;
  SDL_FillRect(screen,&dest,SDL_MapRGB(screen->format,57,133,90));

  // draw ship
  SDL_Rect rboat;
  rboat.x=136;
  rboat.y=24;
  if(boat)
    SDL_BlitSurface(boat,NULL,screen,&rboat);

  // draw waves
  SDL_Rect rwave;
  rwave.w=20;
  rwave.h=1;
  rwave.y=47;
  rwave.x=int(water_wave);
  for(int f=0;f<8;f++)
  {
    SDL_FillRect(screen,&rwave,SDL_MapRGB(screen->format,56,152,255));
    rwave.x+=40;
  }
  if(water_wave>=20)
  {
    rwave.x=0;
    rwave.w=int(water_wave-20);
    SDL_FillRect(screen,&rwave,SDL_MapRGB(screen->format,56,152,255));
  }
  if(program_mode==PROGRAM_MODE_GAME)
    water_wave+=0.4;
  if(water_wave>=40)
    water_wave=0;

  // draw treasures
  SDL_Rect rgold;
  for(int i=0; i<4; i++)
  {
    if(gold_list[i].exist)
    {
      rgold.x=gold_list[i].x;
      rgold.y=gold_list[i].y;
      if(gold)
        SDL_BlitSurface(gold,NULL,screen,&rgold);
    }
  }

  // draw bathyscaphe
  SDL_Rect rship;
  rship.x=ship_x;
  rship.y=ship_y;
  if(ship_disabled)
  {
    if(shipdisabled)
      SDL_BlitSurface(shipdisabled,NULL,screen,&rship);
  }
  else
  {
    if(ship)
      SDL_BlitSurface(ship,NULL,screen,&rship);
  }

  // draw bugs
  SDL_Rect rbug;
  for(int i=0; i<bug_list.size(); i++)
  {
    rbug.x=bug_list[i].x;
    rbug.y=bug_list[i].y;
    if(bug)
      SDL_BlitSurface(bug,NULL,screen,&rbug);
  }

  // draw bubbles
  SDL_Rect rbubble;
  for(int i=0; i<bubble_list.size(); i++)
  {
    rbubble.x=bubble_list[i].x;
    rbubble.y=bubble_list[i].y;
    if(bubble)
      SDL_BlitSurface(bubble,NULL,screen,&rbubble);
  }

  // draw plants
  SDL_Rect rgreen;
  for(int i=0; i<green_list.size(); i++)
  {
    rgreen.x=green_list[i].x;
    rgreen.y=green_list[i].y;
    if(green[int(green_list[i].frame)])
    {
      SDL_BlitSurface(green[int(green_list[i].frame)],NULL,screen,&rgreen);
    }
  }

  // draw clouds
  SDL_Rect rcloud;
  for(int i=0; i<cloud_list.size(); i++)
  {
    rcloud.x=cloud_list[i].x;
    rcloud.y=cloud_list[i].y;
    if(cloud)
      SDL_BlitSurface(cloud,NULL,screen,&rcloud);
  }

  // draw texts
  char txt[20];
  sprintf(txt,lang.get_string(4),level);
  draw_text(screen,txt,10,5,0,0,0);
  sprintf(txt,lang.get_string(5),score);
  draw_text(screen,txt,250,5,0,0,0);
}

void update_game()
{
  if(!ship_disabled)
  {
    // ship impulse
    ship_y+=(int)ship_av;
    ship_x+=(int)ship_ah;
    if(ship_x<0)
      ship_x=0;
    if(ship_x>296)
      ship_x=296;
    if(ship_y<48)
    {
      ship_y=48;
      ship_av=0.1;
    }
    if(ship_y>204)
    {
      ship_y=204;
      ship_av=0;
    }
  }

  // check if get treasure
  for(int i=0; i<4; i++)
  {
    if(gold_list[i].exist)
    {
      if(gold_list[i].carried)
      {
        gold_list[i].x=ship_x;
        gold_list[i].y=ship_y+24;
      }
      if(!ship_load && ship_y>=204 && ship_x>gold_list[i].x-4 && ship_x<gold_list[i].x+4)
      {
        gold_list[i].carried=1;
        ship_load=1;
        Mix_PlayChannel(-1,sound_hit,0);
      }
    }
  }

  // check if rescue treasure
  if(ship_load && ship_y<=48 && ship_x>=136 && ship_x<160)
  {
    for(int i=0; i<4; i++)
    {
      if(gold_list[i].carried)
      {
        gold_list[i].carried=0;
        gold_list[i].exist=0;
        ship_load=0;
        score++;
        Mix_PlayChannel(-1,sound_gold,0);
        switch(score)
        {
          case 1:
            exp_win(1);
            break;
          case 5:
            exp_win(2);
            break;
          case 10:
            exp_win(3);
            break;
          case 20:
            exp_win(4);
            break;
        }
      }
    }
  }

  // if get all treasures, advance to next level
  int boxes=0;
  for(int i=0; i<4; i++)
  {
    if(!gold_list[i].exist)
      boxes++;
  }
  if(boxes==4)
    new_level();

  // move bugs
  for(int i=0; i<bug_list.size(); i++)
  {
    bug_list[i].x+=bug_list[i].dir_x;
    bug_list[i].y+=bug_list[i].dir_y;
    if(bug_list[i].x<0 || bug_list[i].x>298)
      bug_list[i].dir_x=-bug_list[i].dir_x;
    if(bug_list[i].y<48 || bug_list[i].y>204)
      bug_list[i].dir_y=-bug_list[i].dir_y;
    // check collision
    if(!ship_disabled)
      if(bug_list[i].x>ship_x-20 && bug_list[i].x<ship_x+20 && bug_list[i].y>ship_y-20 && bug_list[i].y<ship_y+20)
        finish();
  }

  // move bubbles
  for(int i=0; i<bubble_list.size(); i++)
  {
    bubble_list[i].x+=(int)bubble_list[i].ah-1+rand()%3;
    bubble_list[i].y+=(int)bubble_list[i].av;
    if(bubble_list[i].ah<0)
      bubble_list[i].ah+=0.3;
    if(bubble_list[i].ah>0)
      bubble_list[i].ah-=0.3;
    if(bubble_list[i].av>-1)
      bubble_list[i].av-=0.3;
    if(bubble_list[i].y<48)
    {
      bubble_list.erase(bubble_list.begin()+i);
      i--;
    }
  }

  // move plants
  for(int i=0; i<green_list.size(); i++)
  {
    green_list[i].frame+=0.2;
    if(green_list[i].frame>3.9)
      green_list[i].frame=0;
  }

  // move clouds
  for(int i=0; i<cloud_list.size(); i++)
  {
    if(cloud_list[i].frame>0)
    {
      cloud_list[i].frame+=cloud_list[i].vel;
      if(cloud_list[i].frame>3.9)
      {
        cloud_list[i].frame=0.1;
        cloud_list[i].x+=1;
        if(cloud_list[i].x>319)
          cloud_list[i].x=-48;
      }
    }
    else
    {
      cloud_list[i].frame-=cloud_list[i].vel;
      if(cloud_list[i].frame<-3.9)
      {
        cloud_list[i].frame=-0.1;
        cloud_list[i].x-=1;
        if(cloud_list[i].x<-48)
          cloud_list[i].x=320;
      }
    }
  }

  // check exp times
  if(ship_y==48 || ship_y==204)
    floating_time=SDL_GetTicks();
  if(ship_y!=204)
    floor_time=SDL_GetTicks();
  if((SDL_GetTicks()-floating_time)/1000>=30)
  {
    if(ship_load)
      exp_win(7);
    else
      exp_win(6);
  }
  if((SDL_GetTicks()-floor_time)/1000>=60)
    exp_win(8);

  read_game_keys();
  draw_game();
}

void update_end()
{
  process_events();
  if(mainjoystick.any)
    program_mode=PROGRAM_MODE_MENU;

  draw_game();
  draw_text(screen,lang.get_string(7),151,111,0,0,0);
  draw_text(screen,lang.get_string(7),150,110,255,255,255);
}

void read_pause_keys()
{
  process_events();

  if(mainjoystick.button_a || mainjoystick.button_back)
    program_mode=PROGRAM_MODE_GAME;
}

void draw_pause()
{
  draw_game();
  draw_text(screen,"Pause",140,110,255,255,0);
}

void update_pause()
{
  read_pause_keys();
  draw_pause();
}

///////////////////////////////////
/*  Init                         */
///////////////////////////////////
int main(int argc, char *argv[])
{
  for(int f=0; f<argc; f++)
  {
    if(std::string(argv[f])=="-fullscreen")
      fullscreen=SDL_FULLSCREEN;
    if(std::string(argv[f])=="-scanlines")
      scanlines=1;
  }

  if(SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO | SDL_INIT_AUDIO)<0)
		return 0;

  screen2 = SDL_SetVideoMode(640, 480, 16, SDL_DOUBLEBUF | SDL_SWSURFACE | fullscreen);
  if (screen2==NULL)
    return 0;
  screen=SDL_CreateRGBSurface(SDL_SRCCOLORKEY, 320, 240, 16, 0,0,0,0);
  if(screen==NULL)
    return 0;

  SDL_JoystickEventState(SDL_ENABLE);
  joystick=SDL_JoystickOpen(0);
  SDL_ShowCursor(0);

  init_game();
  load_records();
  init_exp();

  const int GAME_FPS=60;
  Uint32 start_time;

  while(!done)
	{
    start_time=SDL_GetTicks();
    switch(program_mode)
    {
      case PROGRAM_MODE_MENU:
        update_menu();
        break;
      case PROGRAM_MODE_GAME:
        update_game();
        break;
      case PROGRAM_MODE_PAUSE:
        update_pause();
        break;
      case PROGRAM_MODE_END:
        update_end();
        break;
    }

    exp_update();

    filter_surface(screen,screen2);
    SDL_Flip(screen2);

    // set FPS 60
    if(1000/GAME_FPS>SDL_GetTicks()-start_time)
      SDL_Delay(1000/GAME_FPS-(SDL_GetTicks()-start_time));
	}

  end_game();
  save_records();
  exp_end();

#ifdef PLATFORM_GP2X
  // Back to wiz menu
  chdir("/usr/gp2x");
  execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
#endif // PLATFORM_GP2X

	return 1;
}
