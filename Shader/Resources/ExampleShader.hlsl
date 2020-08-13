//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
// You can get syntax highlighting for HLSL files:
//
// 1. Visit Tools|Options, Text Editor\File Extension
//
// 2. Enter `hlsl' in the Extension field, and select `Microsoft Visual
//    C++' from the Editor dropdown. Click Add.
//
// 3. Close and re-open any hlsl files you had open already.
//
// As you might guess, this makes Visual Studio interpret HLSL as C++.
// So it isn't quite perfect, and you get red underlines everywhere.
// On the bright side, you at least get brace matching, and syntax
// highlighting for comments.
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

// See the CommonApp comments for the names of the globals it looks for
// and sets automatically.
cbuffer CommonApp
{
	float4x4 g_WVP;
	float4 g_lightDirections[MAX_NUM_LIGHTS];
	float3 g_lightColours[MAX_NUM_LIGHTS];
	int g_numLights;
	float4x4 g_InvXposeW;
	float4x4 g_W;
};

// Add your own globals into your own cbuffer, or just make them
// global. The globals that are outside any explicit cbuffer go
// into a special cbuffer called "$Globals".
cbuffer MyApp
{
	float	g_frameCount;
	float3	g_waveOrigin;
}

struct VSInput
{
	float4 pos:POSITION;
	float4 colour:COLOUR0;
	float3 normal:NORMAL;
	float2 tex:TEXCOORD;
};

struct PSInput
{
	float4 pos:SV_Position;
	float4 colour:COLOUR0;
	float3 normal:NORMAL;
	float2 tex:TEXCOORD;
	float4 mat:COLOUR1;
};

struct PSOutput
{
	float4 colour:SV_Target;
};

Texture2D g_materialMap;
Texture2D g_texture0;
Texture2D g_texture1;
Texture2D g_texture2;

SamplerState g_sampler;

void VSMain(const VSInput input, out PSInput output)
{
	output.pos = mul(input.pos, g_WVP);
	//output.pos.x += cos(g_frameCount / 4 + input.pos.y / 2 + input.pos.z / 2) * 2;
	//output.pos.y += sin(g_frameCount / 4 + input.pos.x /2 + input.pos.z /2)* 2;
	
	//calculate the position of the current vertex normalize [0-1]
	float matX = (input.pos.x + 512)/1024;
	float matZ = (-input.pos.z + 512)/1024;

	//sample the pixel colour from the material map and pass it through to the pixel shader
	output.mat = g_materialMap.SampleLevel(g_sampler, float2(matX, matZ), 0);

	output.tex = input.tex;

	output.colour = input.colour;
	output.normal = input.normal;
}

void PSMain(const PSInput input, out PSOutput output)
{
	//sample appropiate pixel from textures
	float4 moss = g_texture0.Sample(g_sampler, input.tex);
	float4 grass = g_texture1.Sample(g_sampler, input.tex);
	float4 asphalt = g_texture2.Sample(g_sampler, input.tex);

	//setting black
	output.colour = float4(0, 0, 0, 1);

	output.colour = lerp(output.colour, moss, input.mat.x);
	output.colour = lerp(output.colour, grass, input.mat.y);
	output.colour = lerp(output.colour, asphalt, input.mat.z);

	float4 colour;
	for (int i = 0; i < g_numLights; ++i)
	{
		float diffuse = max(0.0, dot(normalize(g_lightDirections[i].xyz), normalize(input.normal)));
		output.colour *= float4(g_lightColours[i].r, g_lightColours[i].g, g_lightColours[i].b, 1) * diffuse;
		output.colour.w = 1;
	}

}