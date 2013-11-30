#pragma once

class PitchGuidance
{
public:
	PitchGuidance(void);
	~PitchGuidance(void);

	virtual double GetTargetPitch() = 0;
};
