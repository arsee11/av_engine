#ifndef AV_DISPLAYER_H
#define AV_DISPLAYER_H

#include <av_util.h>
#include <SDL.h>

#include <filter.h>

class AVDisplayer: public Filter<AVParam>
{
public:
    void open(int w, int h)
    {
		_width = w;
		_height = h;
        SDL_Window* sdlWindow = SDL_CreateWindow("My Game Window",
                                                 SDL_WINDOWPOS_UNDEFINED,
                                                 SDL_WINDOWPOS_UNDEFINED,
                                                 _width, _height, 0
                                                 );
        
        if (!sdlWindow) {
            fprintf(stderr, "SDL: could not set video mode - exiting\n");
            exit(1);
        }
        
        _sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
        _sdlTexture = SDL_CreateTexture(
                                        _sdlRenderer,
                                        SDL_PIXELFORMAT_RGB24,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        _width,
                                        _height);
        
        _isopen = true;
    }
    
private:
    bool transform(AVParam* p) override
	{
		//cout << "dis: w=" << p->w << "h=" << p->h << "l=" << p->len << endl;
		if (!_isopen)
		{
            		return false;
		}

		SDL_Rect sdlRect;
		sdlRect.x = 0;
		sdlRect.y = 0;
		sdlRect.w = p->w;
		sdlRect.h = p->h;

		SDL_UpdateTexture(_sdlTexture, &sdlRect, p->data_ptr(), p->w);
		SDL_RenderClear(_sdlRenderer);
		SDL_RenderCopy(_sdlRenderer, _sdlTexture, &sdlRect, &sdlRect);
		SDL_RenderPresent(_sdlRenderer);

		SDL_Event event;
		SDL_PollEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			SDL_Quit();
			break;
		default:
			break;
		}

		return true;
	}


private:
	SDL_Texture* _sdlTexture;
	SDL_Renderer* _sdlRenderer;
	int _width=640;
	int _height=480;
	bool _isopen = false;
};
#endif /*AV_DISPLAYER_H*/
