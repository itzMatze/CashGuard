#pragma once
#include "SDL3/SDL_video.h"
#include <string>

struct SDL_Window;
struct SDL_GLContextState;

class Window
{
public:
	Window() = default;
	[[nodiscard]] bool construct(const std::string& title);
	void destruct();
	SDL_WindowID get_id() const;
	void new_frame();
	void end_frame();

private:
	SDL_Window* window;
	SDL_GLContextState* gl_context;
};
