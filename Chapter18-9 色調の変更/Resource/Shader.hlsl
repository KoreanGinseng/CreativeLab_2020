//! コンスタントバッファ
cbuffer cbSceneParam : register(b0)
{
	matrix	mtxView			: packoffset( c0 );
	matrix	mtxProj			: packoffset( c4 );
};

cbuffer cbMeshParam : register( b1 )
{
	float4	cbvOffset		: packoffset( c0 );
	float4	cbvSize			: packoffset( c1 );
	float4	cbtOffset		: packoffset( c2 );
	float4	cbtSize			: packoffset( c3 );
	float4	cbColor			: packoffset( c4 );
	matrix	mtxWorld		: packoffset( c5 );
};

Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

//! 頂点属性
struct InputVS
{
	float4	pos			: POSITION;
    float2	Tex			: TEXCOORD;
};
struct OutputVS
{
	float4	pos			: SV_POSITION;
    float2	Tex			: TEXCOORD0;
	float4	color		: COLOR0;
};

//! 頂点シェーダ
OutputVS RenderVS( InputVS inVert )
{
	OutputVS	outVert;
	
	float3 pv = mul( float4(inVert.pos * cbvSize.xyz + cbvOffset.xyz, 1), mtxWorld );
	float4 Pos = mul(float4(pv, 1), mtxView);
	outVert.pos = mul(Pos, mtxProj);

	outVert.color = cbColor;

    outVert.Tex = inVert.Tex * cbtSize.xy + cbtOffset.xy;

	return outVert;
}

//! ピクセルシェーダ
const float3 monochrome = float3(0.298912, 0.586611, 0.114478);
const float3 sepia = float3(1.07, 0.74, 0.43);
float4 RenderPS( OutputVS inPixel ) : SV_TARGET
{
	float4 ocol = txDiffuse.Sample(samLinear, inPixel.Tex) * inPixel.color;
	float grayColor = dot(ocol.rgb, monochrome);
	//return float4(grayColor, grayColor, grayColor, 1);
	return float4(float3(grayColor, grayColor, grayColor) * sepia, 1);
}

technique11 TShader
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, RenderVS()));
		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderPS()));
		SetComputeShader(NULL);
	}
}
