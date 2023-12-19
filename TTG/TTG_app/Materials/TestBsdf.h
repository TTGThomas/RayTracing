#pragma once

#include "Bsdf.h"

#include "Utils/Math.h"

class TestBsdf : public Bsdf
{
public:
	virtual void processHit(BsdfPayload payload, Ray* ray) override;
};