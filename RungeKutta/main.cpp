#include "ProgMain.h"

int main(int argc, char **argv)
{	
	CProgMain* ProgMain = new CProgMain();
	ProgMain->InitVideo();
	ProgMain->Init();

	while(ProgMain->IsWorking())
	{
		ProgMain->SDLEvents();
		ProgMain->Draw();
		SDL_GL_SwapBuffers();
	}

	delete ProgMain;
	SDL_Quit();
	return 0;
}
