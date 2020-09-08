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
CMeshContainer				gMesh;
//描画ターゲット
CTexture					gTarget;
//シェーダー
CShader						gShader;
CShaderBind_SpriteBase		gShaderBind;

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
	gCamera.LookAt(Vector3(-2.0f,2.0f,-2.0f),Vector3(0,0,0),Vector3(0,1,0));
	gCamera.PerspectiveFov(MOF_ToRadian(60.0f),1024.0f / 768.0f,0.01f,1000.0f);
	gCamera.Update();
	CGraphicsUtilities::SetCamera(&gCamera);

	//メッシュの読み込み
	gMesh.Load("player.mom");

	//描画ターゲットを作成する
	MofU32 sw = g_pGraphics->GetTargetWidth();
	MofU32 sh = g_pGraphics->GetTargetHeight();
	gTarget.CreateTarget(sw, sh, PIXELFORMAT_R8G8B8A8_UNORM, BUFFERACCESS_GPUREADWRITE);

	//シェーダーの読み込み
	gShader.Load("Shader.hlsl");
	gShaderBind.Create(&gShader);
	//シェーダーに２D描画用の行列を設定する
	CGraphicsUtilities::SetScreenSize(sw, sh, &gShaderBind);
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

	//元の描画ターゲットを取得する
	LPRenderTarget pold = g_pGraphics->GetRenderTarget();

	//作成したテクスチャを描画ターゲットとして設定する
	//深度バッファは元の情報をそのまま使用する
	g_pGraphics->SetRenderTarget(gTarget.GetRenderTarget(), g_pGraphics->GetDepthTarget());
	
	//画面のクリア
	g_pGraphics->ClearTarget(0.0f,0.0f,1.0f,1.0f,1.0f,0);
	
	//深度バッファ有効化
	g_pGraphics->SetDepthEnable(TRUE);

	//メッシュの描画
	CMatrix44 matWorld;
	gMesh.Render(matWorld);
	
	//描画ターゲットを元に戻す
	g_pGraphics->SetRenderTarget(pold, g_pGraphics->GetDepthTarget());
	g_pGraphics->SetDepthEnable(FALSE);

	//描画したターゲットをテクスチャとして画面に描画する
	if (!g_pInput->IsKeyHold(MOFKEY_SPACE))
	{
		CGraphicsUtilities::RenderTexture(0, 0, &gTarget);
	}
	else
	{
		//シェーダーに送るバッファを作成
		CGraphicsUtilities::RenderTexture(0, 0, &gTarget, &gShader, &gShaderBind);
	}
	CGraphicsUtilities::RenderString(10, 10, MOF_COLOR_WHITE, "SPACEキーでシェーダー使用");

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
	gMesh.Release();
	gTarget.Release();
	//シェーダーの解放
	gShaderBind.Release();
	gShader.Release();
	return TRUE;
}