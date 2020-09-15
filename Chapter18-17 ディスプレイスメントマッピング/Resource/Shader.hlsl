//! コンスタントバッファ
cbuffer cbSceneParam : register( b0 )
{
	float4	vecViewPos		: packoffset( c0 );
	matrix	mtxView			: packoffset( c1 );
	matrix	mtxProj			: packoffset( c5 );
};

cbuffer cbMeshParam : register( b1 )
{
	matrix	mtxWorld		: packoffset(c0);
	float4	colRevise		: packoffset(c4);
	float4	CoordsRevise	: packoffset(c5);
};

cbuffer cbMaterialParam : register( b2 )
{
	float4	matDiffuse		: packoffset( c0 );
	float4	matAmbient		: packoffset( c1 );
	float4	matSpeculer		: packoffset( c2 );
	float4	matEmissive		: packoffset( c3 );
	float	matPower		: packoffset( c4 );
};

cbuffer cbLightParam : register( b3 )
{
	float3	litDirection	: packoffset( c0 );
	float4	litDiffuse		: packoffset( c1 );
	float4	litAmbient		: packoffset( c2 );
	float4	litSpeculer		: packoffset( c3 );
};

Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register(s0);

//! 頂点属性
struct InputVS
{
	float4	pos			: POSITION;
	float3	normal		: NORMAL;
    float2	Tex			: TEXCOORD;
	float4	color		: COLOR0;
};
struct OutputVS
{
	float4	pos			: SV_POSITION;
	float2	Tex			: TEXCOORD0;
};

//新規のテクスチャオブジェクトの作成
Texture2D txNormal : register( t1 );

//! 頂点シェーダ
OutputVS RenderVS( InputVS inVert )
{
	OutputVS	outVert;

	matrix	mtxVP = mul( mtxView, mtxProj );

	//頂点シェーダーで法線マップを参照して法線方向に頂点座標を移動させる
	float4 Pos = inVert.pos;
	Pos.xyz += txNormal.SampleLevel(samLinear, inVert.Tex, 0).xyz * 0.05f;
	Pos = mul(Pos, mtxWorld);


	outVert.pos = mul( Pos , mtxVP );

	outVert.Tex = inVert.Tex;
	return outVert;
}

//! ピクセルシェーダ
float4 RenderPS( OutputVS inPixel ) : SV_TARGET
{
	//法線をテクスチャから取得する
	//テクスチャの色では０～１なのでー１～１の範囲に変更する
	float4 n = normalize((2.0f * txNormal.Sample(samLinear, inPixel.Tex)) - 1.0f);

	//ハーフランバートライティング
	float l = dot(n, -litDirection);
	l = l * 0.5f + 0.5f;
	l *= l;

	//ライティング結果から色を計算する
	float3 diff = l * litDiffuse.xyz * matDiffuse.xyz;
	float4 LP = float4( saturate( diff ) , matDiffuse.w );
	return LP * txDiffuse.Sample(samLinear, inPixel.Tex);
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
