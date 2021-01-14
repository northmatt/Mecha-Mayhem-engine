#pragma once
#include "Utilities/Scene.h"

class Algo :
    public Scene
{
public:
	Algo(const std::string& name) :
		Scene(name, BLM::GLMzero, false) {}

	virtual void Init(int windowWidth, int windowHeight) override;
	virtual void Update() override;

private:
	entt::entity m_model = entt::null;
	entt::entity m_camera = entt::null;

	float m_rotx = 0;
	float m_roty = 0;

	bool m_anim1 = true;
};

