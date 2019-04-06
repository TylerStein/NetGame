#include "olcPixelGameEngine.h"

class Game : public olc::PixelGameEngine
{
public:
	Game();
	~Game();

protected:
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
	bool OnUserDestroy() override;
};

