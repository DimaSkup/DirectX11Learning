cbuffer ConstantBuffer
{
	matrix World;
	matrix View;
	matrix Projection;

	float4 vLightDir[3];
	float4 vLightColor[3];
	float4 vOutputColor;
};

struct VS_INPUT
{
	float4 Pos	 : POSITION;
	float3 Norm	: NORMAL;
};

struct PS_INPUT
{
	float4 Pos	 : SV_POSITION;
	float3 Norm : TEXCOORD0;
};


PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Norm = mul(input.Norm, World);

	return output;
}

float4 PS( PS_INPUT input ) : SV_Target
{
	float4 finalColor = 0;

	input.Norm = normalize(input.Norm);

	for (int i = 0; i < 3; i++)
	{
		finalColor += saturate(dot((float3)vLightDir[i], input.Norm) * vLightColor[i]);
	}

	return finalColor;

	//return float4(1.0f, 1.0f, 0.0f, 1.0f);
	//return input.Color;
}

float4 PSsolid() : SV_Target
{
	return vOutputColor;
}