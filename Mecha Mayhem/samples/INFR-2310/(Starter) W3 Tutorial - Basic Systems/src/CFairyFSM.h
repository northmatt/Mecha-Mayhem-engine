#pragma once

#include "FSM/FSM.h"
#include "NOU/Entity.h"

namespace nou
{
	class CFairyFSM : public FSM
	{
	public:

		static const std::string idleClip;
		static const std::string scaredClip;
		static const std::string attackClip;

		enum class AnimState
		{
			IDLE = 0,
			SCARED,
			ATTACK
		};

		CFairyFSM(Entity& owner);
		~CFairyFSM() = default;

		CFairyFSM(CFairyFSM&&) = default;
		CFairyFSM& operator=(CFairyFSM&&) = default;

		void SetState(AnimState state);
		void Update();

	private:

		Entity* m_owner;
		AnimState m_state;
	};
}