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
	virtual void ImGuiFunc() override {
		//insert functions here

	}

	struct PlayerChoice {
		CONUSER user = CONUSER::NONE;
		int model = 0;
		int score = 0;
		glm::vec3 colour = BLM::GLMzero;
	};

	static PlayerChoice players[4];
	static size_t playerCount;
	static int scoreGoal;

private:
	void SetDigits(int number, int index);
	void SortPlayers(std::vector<int>& playerIndices);

	//digit2 is leftmost, digit1 is rightmost, only support double digits
	struct DigitData {
		glm::vec3 startingPos = BLM::GLMzero;
		glm::vec3 finalPos =BLM::GLMzero ;
		entt::entity parent = entt::null;
		entt::entity number = entt::null;
		entt::entity digit1 = entt::null;
		entt::entity digit2 = entt::null;
		entt::entity bar = entt::null;
	};
	
	std::vector<int> playerIndexes = {};

	DigitData playerScores[4] = {};
	entt::entity camera = entt::null;
	entt::entity text = entt::null;
	entt::entity playerEnt = entt::null;

	float m_timer = 0.f;

};

inline LeaderBoard::PlayerChoice LeaderBoard::players[4] = {};
inline size_t LeaderBoard::playerCount = 0;
inline int LeaderBoard::scoreGoal = 5;