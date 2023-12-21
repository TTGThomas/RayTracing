#pragma once

#include "Bsdf.h"

#include "Utils/Math.h"
#include "Utils/Random.h"

class BasicBsdf : public Bsdf
{
public:
	virtual void ProcessHit(BsdfPayload& payload, Ray* ray) override;
};