float4 VS(float4 Pos : POSITION) : SV_POSITION
{
	return Pos;
}

float4 PS(float4 Pos : SV_POSITION) : SV_Target
{
	/*
	
	
	float fLimiter = 500.0f;

	float dist = Pos.x*Pos.x + Pos.y*Pos.y;

	dist = (dist % fLimiter) / fLimiter;

	return float4(dist, 0.0f, dist, 1.0f);
	*/
	return float4(1.0f, 1.0f, 0.0f, 1.0f);
}