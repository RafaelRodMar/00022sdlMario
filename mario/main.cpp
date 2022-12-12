#include<SDL.h>
#include<SDL_image.h>
#include<SDL_mixer.h>
#include<iostream>
#include<string>
#include<time.h>

SDL_Window* g_pWindow = 0;
SDL_Renderer* g_pRenderer = 0;

SDL_Texture* loadTexture(std::string fileName, SDL_Renderer* pRenderer)
{
	SDL_Surface* pTempSurface = IMG_Load(fileName.c_str()); //create Surface with contents of fileName

	if (pTempSurface == 0) //exception handle load error
	{
		std::cout << "error loading file: " << fileName << std::endl;
		std::cout << SDL_GetError() << " " << fileName << std::endl;
		return false;
	}

	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(pRenderer, pTempSurface); //Pass Surface to Texture
	SDL_FreeSurface(pTempSurface); //Delete Surface

	if (pTexture == 0) //exception handle transiction Surfacte to Texture
	{
		std::cout << "error creating Texture of file: " << fileName << std::endl;
		return false;
	}

	return pTexture; //save texture.
}

void drawFrameScl(SDL_Texture* textureMap, int x, int y, int srcWidth, int srcHeight, int destWidth, int destHeight, int currentRow, int currentFrame, SDL_Renderer *pRenderer, double angle, int alpha, SDL_RendererFlip flip)
{
	SDL_Rect srcRect; //source rectangle
	SDL_Rect destRect; //destination rectangle

	srcRect.x = srcWidth * currentFrame;
	srcRect.y = srcHeight * (currentRow);
	srcRect.w = srcWidth;
	destRect.w = destWidth;
	srcRect.h = srcHeight;
	destRect.h = destHeight;
	destRect.x = x;
	destRect.y = y;

	SDL_SetTextureAlphaMod(textureMap, alpha);
	SDL_RenderCopyEx(pRenderer, textureMap, &srcRect, &destRect, angle, 0, flip); //Load current frame on the buffer game.
}

// SOUND / MUSIC
Mix_Music* loadMusic(std::string fileName)
{
	Mix_Music* pMusic = Mix_LoadMUS(fileName.c_str());
	if (pMusic == 0)
	{
		std::cout << "Could not load music: ERROR - " << Mix_GetError() << std::endl;
		return false;
	}

	return pMusic;
}


Mix_Chunk* loadSound(std::string fileName)
{
	Mix_Chunk* pChunk = Mix_LoadWAV(fileName.c_str());
	if (pChunk == 0)
	{
		std::cout << "Could not load SFX: ERROR - " << Mix_GetError() << std::endl;
		return false;
	}

	return pChunk;
}

//void playMusic(Mix_Music* mu, int loop)
//{
//	Mix_PlayMusic(m_music[id], loop);
//}
//
//void AssetsManager::playSound(std::string id, int loop)
//{
//	Mix_PlayChannel(-1, m_sfxs[id], loop);
//}

bool isRunning = true;

float offsetX = 0, offsetY = 0;


const int H = 17;
const int W = 150;


std::string TileMap[H] = {
"000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
"0                                                                                                                                                    0",
"0                                                                                    w                                                               0",
"0                   w                                  w                   w                                                                         0",
"0                                      w                                       kk                                                                    0",
"0                                                                             k  k    k    k                                                         0",
"0                      c                                                      k      kkk  kkk  w                                                     0",
"0                                                                       r     k       k    k                                                         0",
"0                                                                      rr     k  k                                                                   0",
"0                                                                     rrr      kk                                                                    0",
"0               c    kckck                                           rrrr                                                                            0",
"0                                      t0                           rrrrr                                                                            0",
"0G                                     00              t0          rrrrrr            G                                                               0",
"0           d    g       d             00              00         rrrrrrr                                                                            0",
"PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
"PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
"PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
};

class PLAYER {

public:

	float dx, dy;
	SDL_FRect rect; //position of the sprite
	bool onGround;
	SDL_Texture* sprite;
	SDL_FRect imgRect; //source of the image
	float currentFrame;

	PLAYER(std::string image)
	{
		sprite = loadTexture(image, g_pRenderer);
		rect = { 100, 180, 16, 16 };

		dx = dy = 0.1;
		currentFrame = 0;
	}


	void update(float time)
	{

		rect.x += dx * time;
		Collision(0);


		if (!onGround) dy = dy + 0.0005*time;
		rect.y += dy * time;
		onGround = false;
		Collision(1);


		currentFrame += time * 0.005;
		if (currentFrame > 3) currentFrame -= 3;

		if (dx > 0) imgRect = { (float)112 + 31 * int(currentFrame), (float)155, (float)16,(float)16 };
		if (dx < 0) imgRect = { (float)112 + 31 * int(currentFrame) + 16, (float)144, (float)-16, (float)16 };

		rect.x = rect.x - offsetX;
		rect.y = rect.y - offsetY;

		dx = 0;
	}


	void Collision(int num)
	{

		for (int i = rect.y / 16; i < (rect.y + rect.h) / 16; i++)
			for (int j = rect.x / 16; j < (rect.x + rect.w) / 16; j++)
			{
				if ((TileMap[i][j] == 'P') || (TileMap[i][j] == 'k') || (TileMap[i][j] == '0') || (TileMap[i][j] == 'r') || (TileMap[i][j] == 't'))
				{
					if (dy > 0 && num == 1)
					{
						rect.y = i * 16 - rect.h;  dy = 0;   onGround = true;
					}
					if (dy < 0 && num == 1)
					{
						rect.y = i * 16 + 16;   dy = 0;
					}
					if (dx > 0 && num == 0)
					{
						rect.x = j * 16 - rect.w;
					}
					if (dx < 0 && num == 0)
					{
						rect.x = j * 16 + 16;
					}
				}

				if (TileMap[i][j] == 'c') {
					// TileMap[i][j]=' '; 
				}
			}
	}

};

class ENEMY
{

public:
	float dx, dy;
	SDL_FRect rect;
	SDL_Texture* sprite;
	SDL_FRect imgRect;
	float currentFrame;
	bool life;


	void set(std::string image, int x, int y)
	{
		sprite = loadTexture(image, g_pRenderer);
		rect = { (float)x, (float)y, (float)16, (float)16 };

		dx = 0.05;
		currentFrame = 0;
		life = true;
	}

	void update(float time)
	{
		rect.x += dx * time;

		Collision();


		currentFrame += time * 0.005;
		if (currentFrame > 2) currentFrame -= 2;

		imgRect = { (float)18 * int(currentFrame), (float)0, (float)16, 16.0 };
		if (!life) imgRect = { 58, 0, 16, 16 };

		rect.x = rect.x - offsetX;
		rect.y = rect.y - offsetY;

	}


	void Collision()
	{

		for (int i = rect.y / 16; i < (rect.y + rect.h) / 16; i++)
			for (int j = rect.x / 16; j < (rect.x + rect.w) / 16; j++)
				if ((TileMap[i][j] == 'P') || (TileMap[i][j] == '0'))
				{
					if (dx > 0)
					{
						rect.x = j * 16 - rect.w; dx *= -1;
					}
					else if (dx < 0)
					{
						rect.x = j * 16 + 16;  dx *= -1;
					}

				}
	}

};

const int FPS = 60;
const int DELAY_TIME = 1000.0f / FPS;

int main(int argc, char* args[])
{
	// attempt to initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		int flags = 0;

		std::cout << "SDL init success\n";
		// init the window
		g_pWindow = SDL_CreateWindow("Mario platformer", 100, 100,
			400, 250, flags);
		if (g_pWindow != 0) // window init success
		{
			std::cout << "window creation success\n";
			g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);
			if (g_pRenderer != 0) // renderer init success
			{
				std::cout << "renderer creation success\n";
				SDL_SetRenderDrawColor(g_pRenderer,
					255, 255, 255, 255);
			}
			else
			{
				std::cout << "renderer init fail\n";
				return false; // renderer init fail
			}
		}
		else
		{
			std::cout << "window init fail\n";
			return false; // window init fail
		}
	}
	else
	{
		std::cout << "SDL init fail\n";
		return false; // SDL init fail
	}

	//player and enemy
	PLAYER Mario("mario_tileset.png");
	ENEMY  enemy;
	enemy.set("mario_tileset.png", 48 * 16, 13 * 16);

	//scenery
	SDL_Texture* tile = loadTexture("mario_tileset.png", g_pRenderer);

	//music and sound
	Mix_Music* music = loadMusic("mario_theme.ogg");
	Mix_PlayMusic(music, 1);
	Mix_Chunk* jumpSound = loadSound("jump.ogg");
	
	srand(time(NULL));

	Uint32 frameStart, frameTime;

	while (isRunning)
	{
		frameStart = SDL_GetTicks(); //tiempo inicial

		//input handler
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				isRunning = false;
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_LEFT:
					Mario.dx = -0.1;
					break;
				case SDLK_RIGHT:
					Mario.dx = 0.1;
					break;
				case SDLK_UP:
					if (Mario.onGround)
					{
						Mario.dy = -0.27;
						Mario.onGround = false;
						Mix_PlayChannel(-1, jumpSound, 0);
					}
					break;
				default:
					break;
				}
				break;

			case SDL_KEYUP:
				break;

			default:
				break;
			}
		}


		//update
		Mario.update(1);
		enemy.update(1);

		SDL_Rect iMarioRect;
		iMarioRect.x = Mario.rect.x;
		iMarioRect.y = Mario.rect.y;
		iMarioRect.w = Mario.rect.w;
		iMarioRect.h = Mario.rect.h;

		SDL_Rect iEnemyRect;
		iEnemyRect.x = enemy.rect.x;
		iEnemyRect.y = enemy.rect.y;
		iEnemyRect.w = enemy.rect.w;
		iEnemyRect.h = enemy.rect.h;

		if ( SDL_HasIntersection(&iMarioRect, &iEnemyRect) )
		{
			if (enemy.life)
			{
				if (Mario.dy > 0) { enemy.dx = 0; Mario.dy = -0.2; enemy.life = false; }
				else SDL_SetTextureColorMod(Mario.sprite, 255, 0, 0);
			}
		}

		if (Mario.rect.x > 200) offsetX = Mario.rect.x - 200;


		//draw
		SDL_SetRenderDrawColor(g_pRenderer, 107, 140, 255, 255);
		SDL_RenderClear(g_pRenderer);

		//draw background
		/*for (int i = 0; i < H; i++)
			for (int j = 0; j < W; j++)
			{
				if (TileMap[i][j] == 'P')  tile.setTextureRect(IntRect(143 - 16 * 3, 112, 16, 16));

				if (TileMap[i][j] == 'k')  tile.setTextureRect(IntRect(143, 112, 16, 16));

				if (TileMap[i][j] == 'c')  tile.setTextureRect(IntRect(143 - 16, 112, 16, 16));

				if (TileMap[i][j] == 't')  tile.setTextureRect(IntRect(0, 47, 32, 95 - 47));

				if (TileMap[i][j] == 'g')  tile.setTextureRect(IntRect(0, 16 * 9 - 5, 3 * 16, 16 * 2 + 5));

				if (TileMap[i][j] == 'G')  tile.setTextureRect(IntRect(145, 222, 222 - 145, 255 - 222));

				if (TileMap[i][j] == 'd')  tile.setTextureRect(IntRect(0, 106, 74, 127 - 106));

				if (TileMap[i][j] == 'w')  tile.setTextureRect(IntRect(99, 224, 140 - 99, 255 - 224));

				if (TileMap[i][j] == 'r')  tile.setTextureRect(IntRect(143 - 32, 112, 16, 16));

				if ((TileMap[i][j] == ' ') || (TileMap[i][j] == '0')) continue;

				tile.setPosition(j * 16 - offsetX, i * 16 - offsetY);
				window.draw(tile);
			}*/

		//draw Mario
		drawFrameScl(Mario.sprite, Mario.imgRect.x, Mario.imgRect.y, Mario.imgRect.w, Mario.imgRect.h, Mario.rect.w, Mario.rect.h, 0, 0, g_pRenderer, 0.0, 255, SDL_FLIP_NONE);

		//draw Enemy
		drawFrameScl(enemy.sprite, enemy.imgRect.x, enemy.imgRect.y, enemy.imgRect.w, enemy.imgRect.h, enemy.rect.w, enemy.rect.h, 0, 0, g_pRenderer, 0.0, 255, SDL_FLIP_NONE);

		SDL_RenderPresent(g_pRenderer); // draw to the screen

		frameTime = SDL_GetTicks() - frameStart; //tiempo final - tiempo inicial

		if (frameTime < DELAY_TIME)
		{
			//con tiempo fijo el retraso es 1000 / 60 = 16,66
			//procesar handleEvents, update y render tarda 1, y hay que esperar 15
			//cout << "frameTime : " << frameTime << "  delay : " << (int)(DELAY_TIME - frameTime) << endl;
			SDL_Delay((int)(DELAY_TIME - frameTime)); //esperamos hasta completar los 60 fps
		}
	}
	
	std::cout << "game closing...\n";

	return 0;
}