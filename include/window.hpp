#pragma once
#include "SDL3/SDL_video.h"
#include <string>

struct ImFont;
struct SDL_Window;
struct SDL_GLContextState;

enum Font : uint32_t
{
	FONT_DEFAULT = 0,
	FONT_COUNT
};

class Window
{
public:
	Window() = default;
	[[nodiscard]] bool construct(const std::string& title);
	void destruct();
	SDL_WindowID get_id() const;
	ImFont* get_font(Font font);
	void new_frame();
	void end_frame();

private:
	SDL_Window* window;
	SDL_GLContextState* gl_context;
	ImFont* fonts[FONT_COUNT];
};
