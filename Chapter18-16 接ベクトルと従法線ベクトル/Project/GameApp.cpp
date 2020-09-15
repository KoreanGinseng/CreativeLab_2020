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
//カメラ角度
CVector3					gCamAngle;
//メッシュ
CMeshContainer				gMesh;
//シェーダー
CShader                     gShader;
CShaderBind_BumpMapping     gShaderBind;

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
	gCamera.LookAt(Vector3(0.0f,0.0f,-2.0f),Vector3(0,0,0),Vector3(0,1,0));
	gCamera.PerspectiveFov(MOF_ToRadian(60.0f),1024.0f / 768.0f,0.01f,1000.0f);
	gCamera.Update();
	CGraphicsUtilities::SetCamera(&gCamera);

	//メッシュの読み込み
	gMesh.Load("Stage/Stage_01_mdl_base.mom");

	//シェーダーの読み込み
	gShader.Load("Shader.hlsl");
	gShaderBind.Create(&gShader);
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

	//マウスによるカメラ回転
	CVector3 vRot;
	if (g_pInput->IsMouseKeyHold(MOFMOUSE_LBUTTON))
	{
		g_pInput->GetMouseMove(vRot);
		vRot *= 0.005f;
	}
	gCamAngle += Vector3(vRot.y,vRot.x,0);

	//カメラの向きを計算
	CMatrix33 matRot;
	matRot.RotationZXY(gCamAngle);
	CVector3 vFront(0,0,1);
	vFront.TransformCoord(matRot,vFront);

	//キーによるカメラ移動
	float speed = 0.1f;
	CVector3 vMove;
	if(g_pInput->IsKeyHold(MOFKEY_LEFT))
	{
		vMove.x -= speed;
	}
	if(g_pInput->IsKeyHold(MOFKEY_RIGHT))
	{
		vMove.x += speed;
	}
	if(g_pInput->IsKeyHold(MOFKEY_DOWN))
	{
		vMove.z -= speed;
	}
	if(g_pInput->IsKeyHold(MOFKEY_UP))
	{
		vMove.z += speed;
	}
	vMove.TransformCoord(matRot,vMove);
	CVector3 vPos = gCamera.GetViewPosition() + vMove;

	//カメラ更新
	gCamera.LookAt(vPos,vPos + vFront,Vector3(0,1,0));
	gCamera.Update();

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
	
	//シェーダーエフェクトを使って描画
	if (g_pInput->IsKeyHold(MOFKEY_SPACE))
	{
		gShader.SetTechnique(0);
	}
	//シンプルなシェーダー描画
	else
	{
		gShader.SetTechnique(1);
	}

	//メッシュの描画
	CMatrix44 matWorld;
	gMesh.Render(matWorld, &gShader, &gShaderBind);

	CGraphicsUtilities::RenderString(10, 10, "'上''下''左''右'キーでカメラ移動");
	CGraphicsUtilities::RenderString(10, 34, "マウス左ドラッグでカメラ回転");
	CGraphicsUtilities::RenderString(10, 58, MOF_COLOR_WHITE, "SPACEキーでシェーダー使用");
	
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
	//メッシュ解放
	gMesh.Release();
	return TRUE;
}