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
//���b�V��
LPGeometry					pGeometry;
//�V�F�[�_�[
CShader						gShader;
CShaderBind_3DPrimitiveBase	gShaderBind;

//�V�F�[�_�[���̃R���X�^���g�o�b�t�@�Ɠ��l�̍\����
struct cbGameParam
{
	Vector4 cblgPotision;
	Vector4 cblgDiffuse;
	Vector4 cblgAttenuation;
};

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
	gCamera.LookAt(Vector3(-2.0f,2.0f,-20.0f),Vector3(0,0,0),Vector3(0,1,0));
	gCamera.PerspectiveFov(MOF_ToRadian(60.0f),1024.0f / 768.0f,0.01f,1000.0f);
	gCamera.Update();
	CGraphicsUtilities::SetCamera(&gCamera);

	//���b�V���̓ǂݍ���
	pGeometry = CGraphicsUtilities::CreatePlaneGeometry(10, 10, 1, 1, TRUE, Vector3(0, 0, 0));

	//�V�F�[�_�[�̓ǂݍ���
	gShader.Load("Shader.hlsl");
	gShaderBind.Create(&gShader);

	//�V�F�[�_�[�Ƃ̃o�b�t�@�̘A�g
	gShaderBind.CreateShaderBuffer("cbGameParam", sizeof(cbGameParam));
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

	//�V�F�[�_�[�ɑ���o�b�t�@���쐬
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

	//�V�F�[�_�[�Ƀo�b�t�@�𑗂�
	gShaderBind.GetShaderBuffer(0)->SetBuffer(&sb);

	//���b�V���̕`��
	CMatrix44 matWorld;
	pGeometry->Render(matWorld, &gShader, &gShaderBind);
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
	//�V�F�[�_�[�̉��
	gShaderBind.Release();
	gShader.Release();
	return TRUE;
}