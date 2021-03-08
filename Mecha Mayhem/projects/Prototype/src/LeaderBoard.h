#pragma once
#include "Utilities/Scene.h"
#include "Utilities/Catmull.h"

class LeaderBoard : public Scene
{
public:
	LeaderBoard(const std::string& name) :
		Scene(name, BLM::GLMzero, false) {}

	virtual void Init(int windowWidth, int windowHeight) override;
	virtual void Update() override;
	virtual void Exit() override;

	virtual Scene* Reattach() override;

	struct PlayerChoice {
		CONUSER user = CONUSER::NONE;
		int model = 0;
		int score = 0;
	};

	static PlayerChoice players[4];
	static size_t playerCount;

private:

	int width = 10;
	int height = 10;


	entt::entity camera = entt::null;
	entt::entity title = entt::null;
	entt::entity text = entt::null;
};

inline LeaderBoard::PlayerChoice LeaderBoard::players[4] = {
	{}, {}, {}, {}
};
inline size_t LeaderBoard::playerCount = 0;