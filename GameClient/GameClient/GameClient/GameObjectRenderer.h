#ifndef GAME_OBJECT_RENDERER
#define GAME_OBJECT_RENDERER

#include "olcPixelGameEngine.h"
#include "GameObject.h"

class GameObjectRenderer
{
public:
	GameObjectRenderer(olc::PixelGameEngine* engine) {
		m_engine = engine;
	}
	~GameObjectRenderer() {
		m_engine = nullptr;
	}

	void Render() {

	}

private:
	olc::PixelGameEngine* m_engine;
};

#endif