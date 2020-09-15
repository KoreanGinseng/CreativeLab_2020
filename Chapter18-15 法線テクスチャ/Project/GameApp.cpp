/*************************************************************************//*!
					
					@file	GameApp.cpp
					@brief	��{�Q�[���A�v���B

															@author	�_�c�@��
															@date	2014.05.14
*//**************************************************************************/

//INCLUDE
#include	"GameApp.h"

//�J����
CCamera						gCamera;
//���C�g
CDirectionalLight			gLight;
//���b�V��
LPGeometry					pGeometry;
//�e�N�X�`��
CTexture					gTexture;
//�V�F�[�_�[
CShader						gShader;
CShaderBind_3DPrimitiveBase	gShaderBind;

/*************************************************************************//*!
		@brief			�A�v���P�[�V�����̏�����
		@param			None
				
		@return			TRUE		����<br>
						����ȊO	���s�A�G���[�R�[�h���߂�l�ƂȂ�
*//**************************************************************************/
MofBool CGameApp::Initialize(void){
	//���\�[�X�z�u�f�B���N�g���̐ݒ�
	CUtilities::SetCurrentDirectory("Resource");
	
	//�J����������
	gCamera.SetViewPort();
	gCamera.LookAt(Vector3(-2.0f,2.0f,2.0f),Vector3(0,0,0),Vector3(0,1,0));
	gCamera.PerspectiveFov(MOF_ToRadian(60.0f),1024.0f / 768.0f,0.01f,1000.0f);
	gCamera.Update();
	CGraphicsUtilities::SetCamera(&gCamera);

	//���C�g������
	gLight.SetDirection(CVector3(0, -1, -1));
	CGraphicsUtilities::SetDirectionalLight(&gLight);

	//���b�V���̓ǂݍ���
	pGeometry = CGraphicsUtilities::CreatePlaneGeometry(3, 3, 1, 1, TRUE, Vector3(0, 0, 0));
	//�e�N�X�`���̓ǂݍ���
	gTexture.Load("isi2_n.png");
	LPTexture pTex = new CTexture();
	pTex->Load("isi2.png");
	pGeometry->GetMaterial()->GetTextureArray()->AddLast(pTex);

	//�V�F�[�_�[�̓ǂݍ���
	gShader.Load("Shader.hlsl");
	gShaderBind.Create(&gShader);

	//�e�N�X�`���I�u�W�F�N�g��ݒ�
	gShaderBind.CreateShaderResource("txNormal");
	
	return TRUE;
}
/*************************************************************************//*!
		@brief			�A�v���P�[�V�����̍X�V
		@param			None
				
		@return			TRUE		����<br>
						����ȊO	���s�A�G���[�R�[�h���߂�l�ƂȂ�
*//**************************************************************************/
MofBool CGameApp::Update(void){
	//�L�[�̍X�V
	g_pInput->RefreshKey();

	return TRUE;
}

/*************************************************************************//*!
		@brief			�A�v���P�[�V�����̕`��
		@param			None
				
		@return			TRUE		����<br>
						����ȊO	���s�A�G���[�R�[�h���߂�l�ƂȂ�
*//**************************************************************************/
MofBool CGameApp::Render(void){
	//�`�揈��
	g_pGraphics->RenderStart();
	//��ʂ̃N���A
	g_pGraphics->ClearTarget(0.0f,0.0f,1.0f,0.0f,1.0f,0);
	
	//�[�x�o�b�t�@�L����
	g_pGraphics->SetDepthEnable(TRUE);
	
	//�J������ݒ�
	gShaderBind.SetCamera(&gCamera);

	//���C�g��ݒ�
	gShaderBind.SetDirectionalLight(&gLight);

	//�e�N�X�`���̐ݒ�
	gShaderBind.GetShaderResource(0)->SetResource(&gTexture);

	//���b�V���̕`��
	CMatrix44 matWorld;
	matWorld.RotationX(MOF_ToRadian(90));
	if (g_pInput->IsKeyHold(MOFKEY_SPACE))
	{
		pGeometry->Render(matWorld, &gShader, &gShaderBind);
	}
	else
	{
		pGeometry->Render(matWorld);
	}


	g_pGraphics->SetDepthEnable(FALSE);
	CGraphicsUtilities::RenderString(10, 10, MOF_COLOR_WHITE, "SPACE�L�[�ŃV�F�[�_�[�g�p");
	
	//�`��̏I��
	g_pGraphics->RenderEnd();
	return TRUE;
}
/*************************************************************************//*!
		@brief			�A�v���P�[�V�����̉��
		@param			None
				
		@return			TRUE		����<br>
						����ȊO	���s�A�G���[�R�[�h���߂�l�ƂȂ�
*//**************************************************************************/
MofBool CGameApp::Release(void){
	MOF_SAFE_DELETE(pGeometry);
	gTexture.Release();
	//�V�F�[�_�[�̉��
	gShaderBind.Release();
	gShader.Release();
	return TRUE;
}