#pragma once
#include "Effects/IlluminationBuffer.h"

//A post effect because easy
class TransparencyLayer : public PostEffect
{
public:
	//Initializes framebuffer
	//Overrides post effect Init
	void Init(unsigned width, unsigned height) override;
	
	//Makes it so apply effect with a PostEffect does nothing for this object
	void ApplyEffect(PostEffect* buffer) override { };
	//Exports to the GBuffer
	void ApplyEffect(PostEffect* buffer, GBuffer* gBuffer);

private:
};