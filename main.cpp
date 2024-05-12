
#include "CommonFunc.h"
#include "BaseObject.h"
#include "Timer.h"

#undef main
SDL_Window* g_window;
SDL_Renderer* g_screen;
const int fps = 50;
const int sprite[]={2,1};

#define GRAV 1
#define CAP_SPEED 20
#define JUMP_SPEED 25
int SPEED = 8;
#define BASE 400
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
int rnd(int l,int r)
{
    return l+rng()%(r-l+1);
}

class Character: public BaseObject
{
public:
	Character()
	{
		y_val_ = 0;
		frame_cur_ = 0;
		on_ground_ = 1;
		rect_.x = 100;
		rect_.y = 400;
		status_ = RUN;
		pre_status_ = RUN;
		up = 0;
	}
	~Character(){;}

	enum STATUS
	{
		RUN = 0,
		JUMP = 1
	};
	void Reset()
	{
		Character tmp;
		*this = tmp;
	}
	void Show(SDL_Renderer* des)
	{
		if(pre_status_ != status_) frame_cur_ = 0;
		else frame_cur_++;
		pre_status_ = status_;
//		cout<<status_<<"\n";
		//cout<<frame_cur_<<"\n";
		if(frame_cur_ == 5*sprite[status_]) frame_cur_ = 0;
		string path = "character/chicken-run-" + to_string(frame_cur_/5) + ".png";
		LoadImg(path, des);

		SDL_Rect renderQuad = {rect_.x, rect_.y, rect_.w, rect_.h};

		SDL_RenderCopyEx(des, p_object_, NULL, &renderQuad, 0, NULL, SDL_FLIP_NONE);
	}
	void HandleInputAction(SDL_Event e)
	{
		if(e.type == SDL_KEYDOWN)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_SPACE:
				up = 1;
				break;

			default:
				break;
			}
		}

	}
	void Logic()
	{
		y_val_ += GRAV;
		if(y_val_ >= CAP_SPEED)
		{
			y_val_ = CAP_SPEED;
		}
		if(y_val_ < -3 && !up )
		{
			y_val_ = -3;
		}
		if(up == 1)
		{
			if(on_ground_ == 1)
			{
				y_val_ += -JUMP_SPEED;
			}

			on_ground_ = 0;
			up = 0;
		}
		rect_.y += y_val_;
		if(rect_.y > 400)
		{
			rect_.y = 400;
			y_val_ = 0;
			on_ground_ = 1;
		}
		else on_ground_ = 0;
	}
// private
	float y_val_;
	int status_;
	int pre_status_;
	int frame_cur_;
	bool on_ground_;
	bool up;
};


bool Init()
{
	bool success = 1;
	int ret = SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO );
	if(ret < 0) return false;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");

	g_window = SDL_CreateWindow(GAME_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if(g_window == NULL)
	{
		success = false;
	}
	else
	{
		g_screen = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
		SDL_SetRenderDrawBlendMode(g_screen, SDL_BLENDMODE_BLEND);
		if(g_screen == NULL)
		{
			success = false;
		}
		else
		{
			SDL_SetRenderDrawColor(g_screen, 255, 255, 255, 255);
			int imgFlags = IMG_INIT_PNG;
			if(!(IMG_Init(imgFlags)&&imgFlags))
			{
				success = 0;
			}

			if(TTF_Init() == -1)
			{
				success = 0;
			}

			if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
			{
				printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
				success = false;
			}
		}
	}
	return success;
}

bool overlap(SDL_Rect a, SDL_Rect b)
{
	if(a.x > b.x + b.w - 1 || b.x > a.x + a.w -1) return false;
	if(a.y + a.h - 1 < b.y || b.y + b.h - 1 <a.y) return false;
	return true;
}

BaseObject g_back, menu, button;
Character chick;
void LoadMedia()
{
	g_back.LoadImg("map/back.png", g_screen);
	chick.LoadImg("character/chicken-run-0.png", g_screen);
	menu.LoadImg("map/menu.png", g_screen);
	button.LoadImg("map/button.png", g_screen);
}

vector<BaseObject> threat_list;

int can_gen = 1;
int nxt = 50;
void Close()
{

}
BaseObject threat;
int dead = 0;
int main(int argc, char *argv[])
{
	if(Init() == false) return -1;
	LoadMedia();
	bool quit = 0;
	SDL_Event e;
	Timer time;
	int scrollingOffset = 0;
	int x,y;
	int difficulty = 100;
	while(!quit)
	{
		time.start();

		if(dead == 0)
		{
			while(SDL_PollEvent(&e) != 0)
			{
				if(e.type == SDL_QUIT)
				{
					quit = true;
				}
				chick.HandleInputAction(e);
			}
			const Uint8* keys = SDL_GetKeyboardState(NULL);
			if(keys[SDL_SCANCODE_SPACE])
			{
				chick.up = 1;
			}
			SDL_SetRenderDrawColor( g_screen, 0xFF, 0xFF, 0xFF, 0xFF );
			SDL_RenderClear( g_screen );


			scrollingOffset -= SPEED;
			if( scrollingOffset < -g_back.rect_.w )
			{
				scrollingOffset = 0;
			}
			g_back.SetPos(scrollingOffset, 0);
			g_back.Render(g_screen);
			g_back.SetPos(scrollingOffset + g_back.rect_.w, 0);
			g_back.Render(g_screen);


			if(can_gen == 1)
			{
				int x = rnd(1,3);
				string path = "map/" + to_string(x) + ".png";

				threat.LoadImg(path, g_screen);
				threat.SetPos(1100, 355);
				can_gen = 0;
			}

			threat.rect_.x -= SPEED;
			if(threat.rect_.x < -threat.rect_.w)
			{
				can_gen = 1;
			}
			threat.Render(g_screen);



			chick.Logic();
			chick.Show(g_screen);

			if(overlap(chick.rect_, threat.rect_))
			{
				dead = 1;
			}

		}
		else
		{
			SDL_SetRenderDrawColor( g_screen, 0xFF, 0xFF, 0xFF, 0xFF );
			SDL_RenderClear( g_screen );
			while(SDL_PollEvent(&e) != 0)
			{
				if(e.type == SDL_QUIT)
				{
					quit = true;
				}
				if(e.type == SDL_MOUSEMOTION) x=e.motion.x, y=e.motion.y;
				if(x>=button.rect_.x && x<=button.rect_.x+button.rect_.w && y>=button.rect_.y && y<=button.rect_.y+button.rect_.h)
				{
					if(e.type == SDL_MOUSEBUTTONDOWN)
					{
						dead = 0;
						chick.Reset();
						threat.SetPos(1100, 355);
					}
				}
			}
			g_back.SetPos(scrollingOffset, 0);
			g_back.Render(g_screen);
			g_back.SetPos(scrollingOffset + g_back.rect_.w, 0);
			g_back.Render(g_screen);

			menu.SetPos((SCREEN_WIDTH - menu.rect_.w)/2, (SCREEN_HEIGHT - menu.rect_.h)/2);
			menu.Render(g_screen);

			button.SetPos((SCREEN_WIDTH - button.rect_.w)/2, (SCREEN_HEIGHT - button.rect_.h)/2 + 70);
			button.Render(g_screen);
		}


		SDL_RenderPresent(g_screen);

		int pass_tick = time.getTicks();
		int time_per_frame = 1000/fps;//ms
		if(pass_tick < time_per_frame)
		{
			SDL_Delay(time_per_frame - pass_tick);
		}
	}

	Close();
}
