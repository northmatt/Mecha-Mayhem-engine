#include "CFairyFSM.h"
#include "Sprites/CSpriteAnimator.h"

namespace nou {

	const std::string CFairyFSM::idleClip = "idle";
	const std::string CFairyFSM::scaredClip = "scared";
	const std::string CFairyFSM::attackClip = "attack";

	CFairyFSM::CFairyFSM(Entity& owner)
	{
		m_owner = &owner;
		m_state = AnimState::IDLE;

		SetState(m_state);
	}

	void CFairyFSM::SetState(AnimState state)
	{
		m_state = state;

		//Gets the animator from the entity.
		auto& animator = m_owner->Get<CSpriteAnimator>();

		ClearTriggers();

		switch (m_state) {
		case AnimState::IDLE:
			animator.PlayLoop(idleClip);
			break;
		case AnimState::SCARED:
			animator.PlayLoop(scaredClip);
			break;
		default:	//AnimState::ATTACK:
			animator.PlayLoop(attackClip);
			break;
		}
	}

	void CFairyFSM::Update()
	{
		auto& animator = m_owner->Get<CSpriteAnimator>();

		switch (m_state) {
		case AnimState::IDLE:

			if (GetVariable("moving"))
				SetState(AnimState::SCARED);
			if (GetVariable("attack"))
				SetState(AnimState::ATTACK);

			break;
		case AnimState::SCARED:

			if (!GetVariable("moving"))
				SetState(AnimState::IDLE);
			if (GetVariable("attack"))
				SetState(AnimState::ATTACK);

			break;
		default:	//AnimState::Attack:
			if (!GetVariable("attack"))
				SetState(AnimState::IDLE);
			break;
		}
	}
}
