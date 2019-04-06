#ifndef GAME_OBJECT
#define GAME_OBJECT

#include "Vector2.h"
#include "olcPixelGameEngine.h"

struct RenderProperties
{
	olc::Pixel color = olc::WHITE;
	int32_t radius = 6;
};

class GameObject
{
public:
	Vector2 Position = Vector2();

	GameObject();
	GameObject(std::string name);
	~GameObject();

	void Translate(Vector2 delta);

	std::string GetName();
	void SetName(std::string name);

	void SetRadius(int32_t radius = 6);
	void SetColor(olc::Pixel color = olc::WHITE);
	void SetRenderProperties(RenderProperties renderProperties);

	void Render(olc::PixelGameEngine& engine, bool displayName = false);

	void IterateColor();

protected:
	std::string m_name = "unnamed";
	RenderProperties m_rendProps;
	size_t m_color_index = 0;
};

static int colors_count = 4;
static olc::Pixel* object_colors = new olc::Pixel[4]{ olc::WHITE, olc::RED, olc::GREEN, olc::BLUE };

#endif