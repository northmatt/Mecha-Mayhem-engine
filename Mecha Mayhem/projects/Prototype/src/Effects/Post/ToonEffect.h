#pragma once
#include "PostEffect.h"

class ToonEffect : public PostEffect
{
public:
	//Initializes framebuffer
	//Overrides post effect Init
	void Init(unsigned width, unsigned height) override;

	//Applies the effect to this buffer
	//passes the previous framebuffer with the texture to apply as parameter
	void ApplyEffect(PostEffect* buffer) override;

	void SetBands(int amt);
	int GetBands() const;

private:
	int _bands = 5;
};
