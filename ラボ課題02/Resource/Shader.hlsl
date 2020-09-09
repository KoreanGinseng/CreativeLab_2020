//! コンスタントバッファ
cbuffer cbSceneParam : register(b0)
{
	matrix	mtxView			: packoffset(c0);
	matrix	mtxProj			: packoffset(c4);
};

cbuffer cbMeshParam : register(b1)
{
	float4	cbvOffset		: packoffset(c0);
	float4	cbvSize			: packoffset(c1);
	float4	cbtOffset		: packoffset(c2);
	float4	cbtSize			: packoffset(c3);
	float4	cbColor			: packoffset(c4);
	matrix	mtxWorld		: packoffset(c5);
};

Texture2D txDiffuse    : register( t0 );
SamplerState samLinear : register( s0 );

cbuffer cbGameParam : register(b2)
{
	float2 ViewportSize;
	float2 Center;
	float  Power;
};


//! 頂点属性
struct InputVS
{
	float4	pos			: POSITION;
    float2	Tex			: TEXCOORD;
};

struct OutputVS
{
	float4	pos			: SV_POSITION;
	float2  Tex         : TEXCOORD0;
};

//! 頂点シェーダ
OutputVS RenderVS( InputVS inVert )
{
	OutputVS	outVert;

	float3 pv   = mul(float4(inVert.pos * cbvSize.xyz + cbvOffset.xyz, 1), mtxWorld);
	float4 Pos  = mul(float4(pv, 1), mtxView);
	outVert.pos = mul(Pos, mtxProj);

	outVert.Tex = inVert.Tex * cbtSize.xy + cbtOffset.xy;

	float2 ViewportOffset = float2(0.5f, 0.5f) / ViewportSize;

	return outVert;
}

//! ピクセルシェーダ
float4 RenderPS( OutputVS inPixel ) : SV_TARGET
{
	float4 color[10];
	float2 ViewportOffset = float2(0.5f, 0.5f) / ViewportSize;
    float2 dir            = Center - inPixel.Tex;
    float len             = length(dir);
    float2 offset         = normalize(dir) * ViewportOffset;
    offset               *= (Power * len);
    
    // ( シーンのテクスチャ,座標+オフセット )*ガウシアンウェイト
    color[0] = txDiffuse.Sample(samLinear, inPixel.Tex                ) * 0.19f;
    color[1] = txDiffuse.Sample(samLinear, inPixel.Tex + offset       ) * 0.17f;
    color[2] = txDiffuse.Sample(samLinear, inPixel.Tex + offset * 2.0f) * 0.15f;
    color[3] = txDiffuse.Sample(samLinear, inPixel.Tex + offset * 3.0f) * 0.13f;
    color[4] = txDiffuse.Sample(samLinear, inPixel.Tex + offset * 4.0f) * 0.11f;
    color[5] = txDiffuse.Sample(samLinear, inPixel.Tex + offset * 5.0f) * 0.09f;
    color[6] = txDiffuse.Sample(samLinear, inPixel.Tex + offset * 6.0f) * 0.07f;
    color[7] = txDiffuse.Sample(samLinear, inPixel.Tex + offset * 7.0f) * 0.05f;
    color[8] = txDiffuse.Sample(samLinear, inPixel.Tex + offset * 8.0f) * 0.03f;
    color[9] = txDiffuse.Sample(samLinear, inPixel.Tex + offset * 9.0f) * 0.01f;
    
    float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    Color = (color[0] + color[1] + color[2] + color[3] + color[4]
	  	   + color[5] + color[6] + color[7] + color[8] + color[9]);
    return Color;
}

technique11 TShader
{
	pass P0
	{		
		  SetVertexShader( CompileShader( vs_4_0, RenderVS() ) );
		  SetGeometryShader( NULL );
		  SetHullShader( NULL );
		  SetDomainShader( NULL );
		  SetPixelShader(  CompileShader( ps_4_0, RenderPS() ) );
		  SetComputeShader( NULL );
	}
}
