#pragma once

#include "Bsdf.h"

#include "Utils/Math.h"

class TestBsdf : public Bsdf
{
public:
	virtual void ProcessHit(BsdfPayload& payload, Ray* ray) override;
};