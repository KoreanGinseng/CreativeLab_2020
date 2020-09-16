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
	Vector4 cblgPosition;
	Vector4 cblgDirection;
	Vector4 cblgDiffuse;
	Vector4 cblgAttenuation;
	float cblgCosTheta;
	float cblgCosPhi;
	float cblgCosFallOff;
	float dummy;
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
	gCamera.LookAt(Vector3(-2.0f,2.0f,-8.0f),Vector3(0,0,0),Vector3(0,1,0));
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
	Vector3 pos        = Vector3(0, 2.5f, 0);
	sb.cblgPosition    = pos;
	sb.cblgDirection   = Vector3(0, -1, 0.6f);
	CVector4Utilities::Normal(sb.cblgDirection, sb.cblgDirection);
	sb.cblgDiffuse     = Vector4(1, 1, 1, 1);
	sb.cblgAttenuation = Vector4(0.0f, 0.1f, 0.05f, 0);
	sb.cblgCosTheta    = cos(MOF_ToRadian(30));
	sb.cblgCosPhi      = cos(MOF_ToRadian(50));
	sb.cblgCosFallOff  = 3.2f;
	
	CGraphicsUtilities::RenderSphere(CSphere(pos, 0.1f), sb.cblgDiffuse);

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