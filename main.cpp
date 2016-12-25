#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>

#if 0
	#define WIDTH 1920
	#define HEIGHT 1080
#else
	#define WINDOWED
	#define WIDTH 1200
	#define HEIGHT 700
#endif

#if USING_SDL1
	#include <SDL/SDL.h>
	#include <SDL/SDL_ttf.h>
	#ifdef WINDOWED
		#define FLAGS SDL_ANYFORMAT
	#else
		#define FLAGS SDL_ANYFORMAT | SDL_FULLSCREEN
	#endif
#else
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_ttf.h>
	#ifdef WINDOWED
		#define FLAGS SDL_WINDOW_SHOWN
	#else
		#define FLAGS SDL_WINDOW_FULLSCREEN_DESKTOP
	#endif
#endif

#include "SU.h"

void DIE(const char* reason)
{
	#if USING_SDL1
		std::cerr << std::endl << "[FATAL ERROR]: " << reason << std::endl << std::endl;
	#else
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
							 "FATAL ERROR",
							 reason,
							 NULL);
	#endif

	exit(1);
}

void DIE()
{
	DIE(SDL_GetError());
}

#if USING_SDL1
std::list<int> pressed_down_keys;
#else
std::list<SDL_Keycode> pressed_down_keys;
#endif




class Scene
{
public:
	SU::Object* obj;
	std::string name;

	Scene(std::string n) : name(n)
	{
		obj = new SU::Object();
		obj->enabled = false;
	}

	virtual void onEnter()
	{
		obj->enabled = true;
	}

	virtual void onLeave()
	{
		obj->enabled = false;
	}

	virtual ~Scene() {}
};

class HouseScene : public Scene
{
public:
	HouseScene() : Scene("House")
	{
		SU::Mesh* mesh = new SU::Mesh();

	// front
	    mesh->add(new SU::Triangle(0, 0, 0,     1, 0, 0,    0, 1, 0,    SU::mapColor(200, 200, 50), true));
	    mesh->add(new SU::Triangle(1, 1, 0,     0, 1, 0,    1, 0, 0,    SU::mapColor(200, 200, 50), true));
	    mesh->add(new SU::Triangle(0, 1, 0,     1, 1, 0,    0.5, 1.5, 0.5,  SU::mapColor(150, 20, 20), true));

	    // right
	    mesh->add(new SU::Triangle(1, 0, 0,     1, 1, 1,    1, 1, 0,    SU::mapColor(200, 200, 50), true));
	    mesh->add(new SU::Triangle(1, 0, 0,     1, 0, 1,    1, 1, 1,    SU::mapColor(200, 200, 50), true));
	    mesh->add(new SU::Triangle(1, 1, 0,     1, 1, 1,    0.5, 1.5, 0.5,  SU::mapColor(150, 20, 20), true));

	    // left
	    mesh->add(new SU::Triangle(0, 0, 0,     0, 1, 0,    0, 1, 1,    SU::mapColor(200, 200, 50), true));
	    mesh->add(new SU::Triangle(0, 0, 0,     0, 1, 1,    0, 0, 1,    SU::mapColor(200, 200, 50), true));
	    mesh->add(new SU::Triangle(0, 1, 1,     0, 1, 0,    0.5, 1.5, 0.5,  SU::mapColor(150, 20, 20), true));

	    // back
	    mesh->add(new SU::Triangle(0, 0, 1,     0, 1, 1,    1, 1, 1,    SU::mapColor(200, 200, 50), true));
	    mesh->add(new SU::Triangle(0, 0, 1,     1, 1, 1,    1, 0, 1,    SU::mapColor(200, 200, 50), true));
	    mesh->add(new SU::Triangle(1, 1, 1,     0, 1, 1,    0.5, 1.5, 0.5,  SU::mapColor(150, 20, 20), true));

	    // bottom
	    mesh->add(new SU::Triangle(0, 0, 0,     0, 0, 1,    1, 0, 0,    SU::mapColor(10, 20, 50)));
	    mesh->add(new SU::Triangle(1, 0, 1,     1, 0, 0,    0, 0, 1,    SU::mapColor(10, 20, 50)));

		obj->mesh = mesh;
		obj->transforming = true;

		obj->position = SU::Vector(-0.5, -0.5, -0.5);
		obj->enabled = false;
	}
};

class SpaceshipScene : public Scene
{
public:
	SU::Mesh hajtomu, torzs, vaz;
    SU::Object torzsObject,torzsTukor,torzsHatul,torzsHatulT,torzslent,tlenttukor,tlenthatul,tlehatu,ship, h1, h2, h3, h4, vaz1, vaz2, vaz3, vaz4;

	SpaceshipScene() : Scene("Spaceship")
	{
	    const int reszletesseg = 20;
	    for (int i = 0; i < reszletesseg; i++)
	    {
	        SU::Vector a(0, 1, 0), b(-2, 0.5, 0), x(1, 0, 0);
	        hajtomu.add(new SU::Quad(b.rotated(x, M_PI / reszletesseg * 2 * (i)),
	                                 a.rotated(x, M_PI / reszletesseg * 2 * (i)),
	                                 a.rotated(x, M_PI / reszletesseg * 2 * (i + 1)),
	                                 b.rotated(x, M_PI / reszletesseg * 2 * (i + 1)), SU::mapColor(30, 30, 30), true));

	        SU::Vector aa(-2, 0.5, 0), bb(-1.9, 0.2, 0);
	        hajtomu.add(new SU::Quad(bb.rotated(x, M_PI / reszletesseg * 2 * (i)),
	                                 aa.rotated(x, M_PI / reszletesseg * 2 * (i)),
	                                 aa.rotated(x, M_PI / reszletesseg * 2 * (i + 1)),
	                                 bb.rotated(x, M_PI / reszletesseg * 2 * (i + 1)), SU::mapColor(250, 50, 50), false));

	        SU::Vector aaa(-1.9, 0.2, 0), bbb(-3, 0, 0);
	        hajtomu.add(new SU::Quad(bbb.rotated(x, M_PI / reszletesseg * 2 * (i)),
	                                 aaa.rotated(x, M_PI / reszletesseg * 2 * (i)),
	                                 aaa.rotated(x, M_PI / reszletesseg * 2 * (i + 1)),
	                                 bbb.rotated(x, M_PI / reszletesseg * 2 * (i + 1)), SU::mapColor(250, 250, 50), false));

	        SU::Vector c(0, 1, 0), d(1, 0.8, 0);
	        hajtomu.add(new SU::Quad(c.rotated(x, M_PI / reszletesseg * 2 * (i)),
	                                 d.rotated(x, M_PI / reszletesseg * 2 * (i)),
	                                 d.rotated(x, M_PI / reszletesseg * 2 * (i + 1)),
	                                 c.rotated(x, M_PI / reszletesseg * 2 * (i + 1)), SU::mapColor(100, 100, 100), true));

	        SU::Vector cc(1, 0.8, 0), dd(1.1, 0.7, 0);
	        hajtomu.add(new SU::Quad(cc.rotated(x, M_PI / reszletesseg * 2 * (i)),
	                                 dd.rotated(x, M_PI / reszletesseg * 2 * (i)),
	                                 dd.rotated(x, M_PI / reszletesseg * 2 * (i + 1)),
	                                 cc.rotated(x, M_PI / reszletesseg * 2 * (i + 1)), SU::mapColor(50, 100, 220), false));

	        SU::Vector ccc(1.1, 0.7, 0), ddd(0.5, 0, 0);
	        hajtomu.add(new SU::Quad(ccc.rotated(x, M_PI / reszletesseg * 2 * (i)),
	                                 ddd.rotated(x, M_PI / reszletesseg * 2 * (i)),
	                                 ddd.rotated(x, M_PI / reszletesseg * 2 * (i + 1)),
	                                 ccc.rotated(x, M_PI / reszletesseg * 2 * (i + 1)), SU::mapColor(30, 30, 30), true));
	    }

	    torzs.add(new SU::Quad(3, 0, 0,     3, 0, 1,        0, 1, 1,        0, 1, 0, SU::mapColor(100, 100, 100), true));
	    torzs.add(new SU::Quad(3, 0, 0,     3.2, -0.5, 0,   3.2, -0.5, 1,   3, 0, 1, SU::mapColor(100, 100, 100), true));
	    torzs.add(new SU::Quad(3, 0, 1,     3.2, -0.5, 1,   0, -0.5, 2,     0, 0, 2, SU::mapColor(30, 30, 30), true));
	    torzs.add(new SU::Triangle(3, 0, 1,     0, 0, 2,        0, 1, 1, SU::mapColor(30, 30, 30), true));


	    torzsObject.mesh = &torzs;
	    torzsObject.transforming = true;

	    torzsTukor.mesh = &torzs;
	    torzsTukor.transforming = true;
	    torzsTukor.flipTriangles = true;
	    torzsTukor.Z.z = -1;

	    torzsHatul.mesh = &torzs;
	    torzsHatul.transforming = true;
	    torzsHatul.flipTriangles = true;
	    torzsHatul.X.x = -2;

	    torzsHatulT.mesh = &torzs;
	    torzsHatulT.transforming = true;
	    torzsHatulT.Z.z = -1;
	    torzsHatulT.X.x = -2;

	    torzslent.mesh = &torzs;
	    torzslent.transforming = true;
	    torzslent.flipTriangles = true;
	    torzslent.Y.y = -0.3;
	    torzslent.position.y = -0.644444;

	    tlenttukor.mesh = &torzs;
	    tlenttukor.transforming = true;
	    tlenttukor.Y.y = -0.3;
	    tlenttukor.Z.z = -1;
	    tlenttukor.position.y = -0.644444;

	    tlenthatul.mesh = &torzs;
	    tlenthatul.transforming = true;
	    tlenthatul.Y.y = -0.3;
	    tlenthatul.X.x = -2;
	    tlenthatul.position.y = -0.644444;

	    tlehatu.mesh = &torzs;
	    tlehatu.transforming = true;
	    tlehatu.Y.y = -0.3;
	    tlehatu.X.x = -2;
	    tlehatu.Z.z = -1;
	    tlehatu.flipTriangles = true;
	    tlehatu.position.y = -0.644444;


	    // BUG! FIXME! gyermek object nem forog, csak ha o is transforming. pedig ettol nem kene fuggnie
	    h1.mesh = &hajtomu;
	    h1.transforming = true;
	    h1.position = SU::Vector(0, 3, 4);
	    h2.mesh = &hajtomu;
	    h2.transforming = true;
	    h2.position = SU::Vector(0, 3, -4);
	    h3.mesh = &hajtomu;
	    h3.transforming = true;
	    h3.position = SU::Vector(0, -3, -4);
	    h4.mesh = &hajtomu;
	    h4.transforming = true;
	    h4.position = SU::Vector(0, -3, 4);

	    vaz.add(new SU::Quad(-1, -1, -1,    1, -1, -1,      1, 1, -1,       -1, 1, -1,  SU::mapColor(30, 30, 30), true));
	    vaz.add(new SU::Quad(-1, -1, 1,     -1, 1, 1,       1, 1, 1,        1, -1, 1,   SU::mapColor(30, 30, 30), true));
	    vaz.add(new SU::Quad(-1, -1, 1,     -1, -1, -1,     -1, 1, -1,      -1, 1, 1,   SU::mapColor(100, 100, 100), true));
	    vaz.add(new SU::Quad(1, -1, -1,     1, -1, 1,       1, 1, 1,        1, 1, -1,   SU::mapColor(100, 100, 100), true));

	    vaz1.mesh = &vaz;
	    vaz1.transforming = true;
	    vaz1.position = SU::Vector(0, 1.3, 2.6);
	    vaz1.Y.y = 6;
	    vaz1.scale(0.21);
	    vaz1.rotateAroundX(1.4);

	    vaz2.mesh = &vaz;
	    vaz2.transforming = true;
	    vaz2.position = SU::Vector(0, 1.3, -2.6);
	    vaz2.Y.y = 6;
	    vaz2.scale(0.21);
	    vaz2.rotateAroundX(-1.4);

	    vaz3.mesh = &vaz;
	    vaz3.transforming = true;
	    vaz3.position = SU::Vector(0, -1.45, -2.65);
	    vaz3.Y.y = 5;
	    vaz3.scale(0.21);
	    vaz3.rotateAroundX(1.4);

	    vaz4.mesh = &vaz;
	    vaz4.transforming = true;
	    vaz4.position = SU::Vector(0, -1.45, 2.65);
	    vaz4.Y.y = 5;
	    vaz4.scale(0.21);
	    vaz4.rotateAroundX(-1.4);



	    ship.addChild(&h1);
	    ship.addChild(&h2);
	    ship.addChild(&h3);
	    ship.addChild(&h4);
	    ship.addChild(&torzsObject);
	    ship.addChild(&torzsTukor);
	    ship.addChild(&torzsHatul);
	    ship.addChild(&torzsHatulT);
	    ship.addChild(&torzslent);
	    ship.addChild(&tlenttukor);
	    ship.addChild(&tlenthatul);
	    ship.addChild(&tlehatu);
	    ship.addChild(&vaz1);
	    ship.addChild(&vaz2);
	    ship.addChild(&vaz3);
	    ship.addChild(&vaz4);

	    ship.transforming = true;
	    ship.scale(0.3);

	    obj->addChild(&ship);
	}

	~SpaceshipScene() {}
};

class SphereTriangle
{
public:
    SU::Vector A, B, C;
    SU::Object* obj;

    SphereTriangle(SU::Vector a, SU::Vector b, SU::Vector c) : A(a), B(b), C(c)
    {
        SU::Mesh* m = new SU::Mesh();
        m->add(new SU::Triangle(A, B, C, SU::mapColor(rand() % 256,rand()%256,rand()%256), true));
        obj = new SU::Object();
        obj->mesh = m;
    }

    void disable()
    {
    	obj->enabled = false;
    }

    ~SphereTriangle() {}
};

class Sphere
{
public:
    SU::Vector top, bottom;
    std::vector<SU::Vector> topRow, bottomRow;
    SU::Object obj;

    std::vector<SphereTriangle*> spts;

    Sphere()
    {
        top = SU::Vector(0, 1, 0);
        for (int i = 0; i < 5; ++i)
        {
            SU::Vector forward(1, 0, 0);
            SU::Vector up_a_bit = forward.rotated(SU::Vector(0, 0, 1), M_PI / 6);
            SU::Vector rotated = up_a_bit.rotated(SU::Vector(0, 1, 0), (M_PI * 2 / 5) * i);
            topRow.push_back(rotated);
            bottomRow.push_back(-1 * rotated);
        }
        bottom = SU::Vector(0, -1, 0);

        for (int i = 0; i < 5; ++i)
        {
            SphereTriangle* spt = new SphereTriangle(top, topRow[i], topRow[i-1<0?4:i-1]);
            obj.addChild(spt->obj);
            spts.push_back(spt);
        }
        for (int i = 0; i < 5; ++i)
        {
            SphereTriangle* spt = new SphereTriangle(bottom, bottomRow[i], bottomRow[i+1>4?0:i+1]);
            obj.addChild(spt->obj);
            spts.push_back(spt);
        }

        for (int i = 0; i < 5; ++i)
        {
            SphereTriangle* spt = new SphereTriangle(bottomRow[(i+1)%5], bottomRow[i], topRow[(i+1+2)%5]);
            obj.addChild(spt->obj);
            spts.push_back(spt);
        }

        for (int i = 0; i < 5; ++i)
        {
            SphereTriangle* spt = new SphereTriangle(topRow[i], topRow[(i+1)%5], bottomRow[(i+1+2)%5]);
            obj.addChild(spt->obj);
            spts.push_back(spt);
        }
    }

    void refine()
    {
        for(auto ch : obj.children)
        {
            ch->enabled = false;
        }

        std::vector<SphereTriangle*> refined_spts;

        for(auto spt : spts)
        {
        	spt->disable();

            SphereTriangle* spt1 = new SphereTriangle(
                spt->A,
                ((spt->A+spt->B)/2).getNormalized(),
                ((spt->A+spt->C)/2).getNormalized());
            obj.addChild(spt1->obj);

            SphereTriangle* spt2 = new SphereTriangle(
                spt->B,
                ((spt->C+spt->B)/2).getNormalized(),
                ((spt->A+spt->B)/2).getNormalized());
            obj.addChild(spt2->obj);

            SphereTriangle* spt3 = new SphereTriangle(
                spt->C,
                ((spt->A+spt->C)/2).getNormalized(),
                ((spt->B+spt->C)/2).getNormalized());
            obj.addChild(spt3->obj);

            SphereTriangle* spt4 = new SphereTriangle(
                ((spt->C+spt->A)/2).getNormalized(),
                ((spt->A+spt->B)/2).getNormalized(),
                ((spt->B+spt->C)/2).getNormalized());
            obj.addChild(spt4->obj);

            refined_spts.push_back(spt1);
            refined_spts.push_back(spt2);
            refined_spts.push_back(spt3);
            refined_spts.push_back(spt4);
        }

        spts.empty();
        spts = refined_spts;

    }

    ~Sphere() {}

};

class IcosahedronScene : public Scene
{
public:
	IcosahedronScene(SU::Object* sphereObj) : Scene("Icosahedron")
	{
		//delete obj;
		obj = sphereObj;
		obj->enabled = false;
	}

	~IcosahedronScene() {}
};

std::vector<Scene*> scenes;
std::vector<Scene*>::iterator currentScene;

struct
{
	double distance, flatAngle, heightAngle;
} Camera;



int main( int argc, char* args[] )
{
	Uint32 currentSec = 0;
	Uint32 currentSecFPS = 0;
	Uint8 previousSecFPS = 0;

	SDL_Init(SDL_INIT_VIDEO);
	if(TTF_Init() == -1)
		DIE(TTF_GetError());
	atexit(SDL_Quit);

	bool running = true;
	bool fps_visible = true;
	bool debug_text_visible = true;
	std::stringstream text;


	#if USING_SDL1
		SDL_Surface *surface = SDL_SetVideoMode(WIDTH, HEIGHT, 32, FLAGS);
	#else
		SDL_Window* 	window;
		SDL_Renderer* 	renderer;
		SDL_Texture* 	texture;
		SDL_Surface* 	surface;

		SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, FLAGS, &window, &renderer);

		texture = SDL_CreateTexture(renderer,
								   SDL_PIXELFORMAT_ARGB8888,
								   SDL_TEXTUREACCESS_STREAMING,
								   WIDTH, HEIGHT);

		surface = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32,
											0x00FF0000,
											0x0000FF00,
											0x000000FF,
											0xFF000000);
	#endif

	SDL_ShowCursor(SDL_DISABLE);
	#if USING_SDL1
	SDL_WarpMouse(WIDTH / 2, HEIGHT / 2);
	#else
	SDL_WarpMouseInWindow(window, WIDTH / 2, HEIGHT / 2);
	#endif

	SU::init(surface);
	SU::setFlag(SU::Flags::LIGHTING);


	TTF_Font *font = TTF_OpenFont("./Dina_r400-10.bdf", 0);
	if (font == NULL)
		DIE(TTF_GetError());

	Sphere sp1;
	scenes.push_back(new IcosahedronScene(&(sp1.obj)));
	scenes.push_back(new SpaceshipScene());
	scenes.push_back(new HouseScene());

	currentScene = scenes.begin();
	(*currentScene)->onEnter();

	Camera.distance = 5;
	Camera.flatAngle = 0;
	Camera.heightAngle = 0;

	while (running)
	{
		SU::Camera::position = SU::Vector(-1 * Camera.distance, 0, 0).rotated(
			SU::Vector(0, 1, 0), Camera.flatAngle);
		SU::Camera::lookDirection = (-1 * SU::Camera::position).getNormalized();
		SU::Camera::upDirection = SU::Vector(0, 1, 0);

		SU::Vector right = SU::Camera::upDirection.crossProduct(SU::Camera::lookDirection);

		SU::Camera::upDirection = SU::Camera::upDirection.rotated(right, Camera.heightAngle);
		SU::Camera::lookDirection = SU::Camera::lookDirection.rotated(right, Camera.heightAngle);
		SU::Camera::position = SU::Camera::position.rotated(right, Camera.heightAngle);

		if (SDL_GetTicks() / 1000 != currentSec)
		{
			currentSec++;
			previousSecFPS = currentSecFPS;
			currentSecFPS = 0;
		}

		SDL_Event e;

		while(SDL_PollEvent(&e) != 0)
		{
			switch (e.type)
			{
				case SDL_QUIT:
				{
					running = false;
				}
				break;

				case SDL_KEYUP:
				{
					pressed_down_keys.remove(e.key.keysym.sym);
				}
				break;

				case SDL_KEYDOWN:
				{
					#if USING_SDL1
					if (true)
					#else
					if (e.key.repeat == 0)
					#endif
					{
						switch (e.key.keysym.sym)
						{
							case SDLK_ESCAPE:
							case SDLK_q:
								running = false;
							break;

							case SDLK_j:
							{
								(*currentScene)->onLeave();
								if (currentScene == scenes.begin())
									currentScene = scenes.end();
								currentScene--;
								(*currentScene)->onEnter();
							}
							break;

							case SDLK_k:
							{
								(*currentScene)->onLeave();
								currentScene++;
								if (currentScene == scenes.end())
									currentScene = scenes.begin();
								(*currentScene)->onEnter();
							}
							break;

							case SDLK_r:
							{
								sp1.refine();
							}
							break;

							case SDLK_F12:
								debug_text_visible = !debug_text_visible;
							break;

							case SDLK_F1:
								SU::toggleFlag(SU::Flags::DEBUG_WIREFRAMING);
							break;

							case SDLK_F2:
								SU::toggleFlag(SU::Flags::DEBUG_TRANSFORMATIONS);
							break;

							case SDLK_F3:
								SU::toggleFlag(SU::Flags::DEBUG_TRANSLATIONS);
							break;

							case SDLK_F4:
								SU::toggleFlag(SU::Flags::ONLY_FACING_TRIANGLES);
							break;

							case SDLK_F5:
								SU::toggleFlag(SU::Flags::DEPTH_SORT);
							break;

							case SDLK_F6:
								SU::toggleFlag(SU::Flags::LIGHTING);
							break;

							default:
								pressed_down_keys.push_back(e.key.keysym.sym);
							break;
						}
					}
				}
				break;

				case SDL_MOUSEMOTION:
				{
					int xdiff = e.motion.x - WIDTH / 2;
					int ydiff = e.motion.y - HEIGHT / 2;

					Camera.heightAngle += ydiff * 0.01;
					Camera.flatAngle += xdiff * 0.01;


					#if USING_SDL1
					SDL_WarpMouse(WIDTH / 2, HEIGHT / 2);
					#else
					SDL_WarpMouseInWindow(window, WIDTH / 2, HEIGHT / 2);
					#endif
				}
				break;

				case SDL_MOUSEBUTTONDOWN:
				{
					switch(e.button.button)
					{
						case SDL_BUTTON_WHEELUP:
							Camera.distance *= 0.9;
						break;

						case SDL_BUTTON_WHEELDOWN:
							Camera.distance /= 0.9;
						break;

						default: break;
					}
				}
				break;

				default: break;
			}
		}

		#if USING_SDL1
		for(int k : pressed_down_keys)
		#else
		for(SDL_Keycode k : pressed_down_keys)
		#endif
		{
			switch(k)
			{
				default: break;
			}
		}

		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));

		SU::render();
		currentSecFPS++;

		SDL_Color c = {150, 150, 150};
		if (fps_visible)
		{
			text.str(std::string());
			text.clear();

			text << (int)previousSecFPS << " FPS --- Scene: " << (*currentScene)->name;

			SDL_Surface *textSurface = TTF_RenderText_Solid(font, text.str().c_str(), c);

			if (textSurface != NULL)
			{
				SDL_Rect r;
				r.x = 10;
				r.y = 10;
				SDL_BlitSurface(textSurface, NULL, surface, &r);
				SDL_FreeSurface(textSurface);
			}
		}

		if(debug_text_visible)
		{
			text.str(std::string());
			text.clear();

			text << "F1 [" << (SU::hasFlag(SU::Flags::DEBUG_WIREFRAMING) ? '+' : ' ' ) << "] DEBUG_WIREFRAMING" << std::endl <<
					"F2 [" << (SU::hasFlag(SU::Flags::DEBUG_TRANSFORMATIONS) ? '+' : ' ' ) << "] DEBUG_TRANSFORMATIONS" << std::endl <<
					"F3 [" << (SU::hasFlag(SU::Flags::DEBUG_TRANSLATIONS) ? '+' : ' ' ) << "] DEBUG_TRANSLATIONS" << std::endl <<
					"F4 [" << (SU::hasFlag(SU::Flags::ONLY_FACING_TRIANGLES) ? '+' : ' ' ) << "] ONLY_FACING_TRIANGLES" << std::endl <<
					"F5 [" << (SU::hasFlag(SU::Flags::DEPTH_SORT) ? '+' : ' ' ) << "] DEPTH_SORT" << std::endl <<
					"F6 [" << (SU::hasFlag(SU::Flags::LIGHTING) ? '+' : ' ' ) << "] LIGHTING" << std::endl <<
					"F7 [" << (SU::hasFlag(SU::Flags::DEBUG_BUMPMAP) ? '+' : ' ' ) << "] BUMPMAP";

			std::string line;
			SDL_Rect r;
			r.x = 10;
			r.y = surface->h - 25;
			while(std::getline(text, line, '\n'))
			{
				SDL_Surface *textSurface = TTF_RenderText_Solid(font, line.c_str(), c);
				if (textSurface != NULL)
				{
					SDL_BlitSurface(textSurface, NULL, surface, &r);
					SDL_FreeSurface(textSurface);
				}

				r.y -= 20;
			}
		}

		#if USING_SDL1
			SDL_Flip(surface);
		#else
			SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
		#endif

		SDL_Delay(1);
	}

	return 0;
}