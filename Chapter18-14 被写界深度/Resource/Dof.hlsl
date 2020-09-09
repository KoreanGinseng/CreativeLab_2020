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
Texture2D txDepth   : register( t1 );

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
float4 RenderPS( OutputVS inPixel ) : SV_TARGET
{
	// 深度値の取得
	float d = txDepth.Sample(samLinear, inPixel.Tex).x;
	// 深度が以上の場合
	if (d >= 1)
	{
		//25サンプリングのぼかし
		float4 color = txDiffuse.Sample(samLinear, inPixel.Tex, float2(-2, -2));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2(-2, -1));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2(-2,  0));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2(-2,  1));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2(-2,  2));

		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2(-1, -2));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2(-1, -1));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2(-1,  0));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2(-1,  1));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2(-1,  2));

		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2( 0, -2));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2( 0, -1));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2( 0,  0));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2( 0,  1));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2( 0,  2));
		
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2( 1, -2));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2( 1, -1));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2( 1,  0));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2( 1,  1));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2( 1,  2));
																   
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2( 2, -2));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2( 2, -1));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2( 2,  0));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2( 2,  1));
		color += txDiffuse.Sample(samLinear, inPixel.Tex, float2( 2,  2));

		color /= 25.0f;

		return color * inPixel.color;
	}

	//通常出力
	return txDiffuse.Sample(samLinear, inPixel.Tex) * inPixel.color;
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
