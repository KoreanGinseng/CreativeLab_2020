/*************************************************************************//*!
					
					@file	GameApp.cpp
					@brief	基本ゲームアプリ。

															@author	濱田　享
															@date	2014.05.14
*//**************************************************************************/

//INCLUDE
#include	"GameApp.h"

//カメラ
CCamera						gCamera;
//メッシュ
LPGeometry					pGeometry;
//シェーダー
CShader						gShader;
CShaderBind_3DPrimitiveBase	gShaderBind;

//シェーダー側のコンスタントバッファと同様の構造体
struct cbGameParam
{
	Vector4 cblgPotision;
	Vector4 cblgDiffuse;
	Vector4 cblgAttenuation;
};

/*************************************************************************//*!
		@brief			アプリケーションの初期化
		@param			None
				
		@return			TRUE		成功<br>
						それ以外	失敗、エラーコードが戻り値となる
*//**************************************************************************/
MofBool CGameApp::Initialize(void){
	//リソース配置ディレクトリの設定
	CUtilities::SetCurrentDirectory("Resource");
	
	//カメラ初期化
	gCamera.SetViewPort();
	gCamera.LookAt(Vector3(-2.0f,2.0f,-20.0f),Vector3(0,0,0),Vector3(0,1,0));
	gCamera.PerspectiveFov(MOF_ToRadian(60.0f),1024.0f / 768.0f,0.01f,1000.0f);
	gCamera.Update();
	CGraphicsUtilities::SetCamera(&gCamera);

	//メッシュの読み込み
	pGeometry = CGraphicsUtilities::CreatePlaneGeometry(10, 10, 1, 1, TRUE, Vector3(0, 0, 0));

	//シェーダーの読み込み
	gShader.Load("Shader.hlsl");
	gShaderBind.Create(&gShader);

	//シェーダーとのバッファの連携
	gShaderBind.CreateShaderBuffer("cbGameParam", sizeof(cbGameParam));
	return TRUE;
}
/*************************************************************************//*!
		@brief			アプリケーションの更新
		@param			None
				
		@return			TRUE		成功<br>
						それ以外	失敗、エラーコードが戻り値となる
*//**************************************************************************/
MofBool CGameApp::Update(void){
	//キーの更新
	g_pInput->RefreshKey();

	return TRUE;
}

/*************************************************************************//*!
		@brief			アプリケーションの描画
		@param			None
				
		@return			TRUE		成功<br>
						それ以外	失敗、エラーコードが戻り値となる
*//**************************************************************************/
MofBool CGameApp::Render(void){
	//描画処理
	g_pGraphics->RenderStart();
	//画面のクリア
	g_pGraphics->ClearTarget(0.0f,0.0f,1.0f,0.0f,1.0f,0);
	
	//深度バッファ有効化
	g_pGraphics->SetDepthEnable(TRUE);
	
	//カメラを設定
	gShaderBind.SetCamera(&gCamera);

	//シェーダーに送るバッファを作成
	cbGameParam sb;
	static float st = 0.0f;
	st += 0.01f;
	sb.cblgPotision    = Vector3(0, 5 + sin(st) * 5, 0);
	sb.cblgDiffuse     = Vector4(1, 1, 1, 1);
	sb.cblgAttenuation = Vector4(0.0f, 0.1f, 0.05f, 0);

	CMatrix44 matWorld2;
	matWorld2.Scaling(0.5f);
	matWorld2.SetTranslation(CVector3(sb.cblgPotision));
	CGraphicsUtilities::RenderSphere(matWorld2);

	//シェーダーにバッファを送る
	gShaderBind.GetShaderBuffer(0)->SetBuffer(&sb);

	//メッシュの描画
	CMatrix44 matWorld;
	pGeometry->Render(matWorld, &gShader, &gShaderBind);
	//描画の終了
	g_pGraphics->RenderEnd();
	return TRUE;
}
/*************************************************************************//*!
		@brief			アプリケーションの解放
		@param			None
				
		@return			TRUE		成功<br>
						それ以外	失敗、エラーコードが戻り値となる
*//**************************************************************************/
MofBool CGameApp::Release(void){
	MOF_SAFE_DELETE(pGeometry);
	//シェーダーの解放
	gShaderBind.Release();
	gShader.Release();
	return TRUE;
}