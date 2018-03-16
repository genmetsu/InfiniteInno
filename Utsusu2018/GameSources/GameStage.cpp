/*!
@file GameStage.cpp
@brief �Q�[���X�e�[�W����
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	�Q�[���X�e�[�W�N���X����
	//--------------------------------------------------------------------------------------
	//�r���[�ƃ��C�g�̍쐬
	void GameStage::CreateViewLight() {
		auto PtrView = CreateView<SingleView>();
		//�r���[�̃J�����̐ݒ�
		auto PtrLookAtCamera = ObjectFactory::Create<MyCamera>();
		PtrView->SetCamera(PtrLookAtCamera);
		PtrLookAtCamera->SetEye(Vec3(0.0f, 5.0f, -5.0f));
		PtrLookAtCamera->SetAt(Vec3(0.0f, 0.0f, 0.0f));

		PtrLookAtCamera->SetRotSpeed(3.0f);
		PtrLookAtCamera->SetLRBaseMode(false);
		PtrLookAtCamera->SetUDBaseMode(false);
		////�V���O�����C�g�̍쐬
		//auto PtrSingleLight = CreateLight<SingleLight>();
		////���C�g�̐ݒ�
		//PtrSingleLight->GetLight().SetPositionToDirectional(-0.001f, 1.0f, -0.001f);

		//�}���`���C�g�̍쐬
		auto PtrMultiLight = CreateLight<MultiLight>();
		//�f�t�H���g�̃��C�e�B���O���w��
		PtrMultiLight->SetDefaultLighting();
		PtrMultiLight->SetAmbientLightColor(Col4(0.08f, 0.04f, 0.0f, 1));
	}

	//�v���[�g�̍쐬
	void GameStage::CreatePlate() {
		//�X�e�[�W�ւ̃Q�[���I�u�W�F�N�g�̒ǉ�
		auto Ptr = AddGameObject<GameObject>();
		auto PtrTrans = Ptr->GetComponent<Transform>();
		Quat Qt(Vec3(1.0f, 0, 0), XM_PIDIV2);
		Qt.rotationRollPitchYawFromVector(Vec3(XM_PIDIV2, 0, 0));
		PtrTrans->SetScale(100.0f, 100.0f, 1.0f);
		PtrTrans->SetQuaternion(Qt);
		PtrTrans->SetPosition(0.0f, 0.0f, 0.0f);

		auto ColPtr = Ptr->AddComponent<CollisionRect>();
		//�`��R���|�[�l���g�̒ǉ�
		auto DrawComp = Ptr->AddComponent<BcPNTStaticDraw>();
		//�`��R���|�[�l���g�Ɍ`��i���b�V���j��ݒ�
		DrawComp->SetMeshResource(L"DEFAULT_SQUARE");
		DrawComp->SetFogEnabled(false);
		//�����ɉe���f�肱�ނ悤�ɂ���
		DrawComp->SetOwnShadowActive(true);

		//�`��R���|�[�l���g�e�N�X�`���̐ݒ�
		DrawComp->SetTextureResource(L"SKY_TX");


		//�ǂ̍쐬
		//�z��̏�����
		vector< vector<Vec3> > Vec = {
			{
				Vec3(0.5f, 0.5f, 100.0f),
				Vec3(0.0f, 0.0f, 0.0f),
				Vec3(50.0f, 0.25f, 0.0f)
			},
			{
				Vec3(0.5f, 0.5f, 100.0f),
				Vec3(0.0f, 0.0f, 0.0f),
				Vec3(-50.0f, 0.25f, 0.0f)
			},

			{
				Vec3(100.0f, 0.5f, 0.5f),
				Vec3(0, 0, 0),
				Vec3(0.0f, 0.25f, 50.0f)
			},
			{
				Vec3(100.0f, 0.5f, 0.5f),
				Vec3(0, 0, 0),
				Vec3(0.0f, 0.25f, -50.0f)
			}
		};
		//�I�u�W�F�N�g�̍쐬
		for (auto v : Vec) {
			AddGameObject<FixedBox>(v[0], v[1], v[2]);
		}

	}

	//�v���C���[�̍쐬
	void GameStage::CreatePlayer() {

		CreateSharedObjectGroup(L"AttackBall");
		//�A�^�b�N�{�[����50�p�ӂ���
		for (int i = 0; i < 50; i++) {
			AddGameObject<AttackBall>();
		}

		//�v���[���[�̍쐬
		auto PlayerPtr = AddGameObject<Player>();
		//�V�F�A�z��Ƀv���C���[��ǉ�
		SetSharedGameObject(L"Player", PlayerPtr);

		AddGameObject<CameraTargetPoint>();
	}

	void GameStage::CreateEnemy() {
		//�z��̏�����
		vector<Vec3> Vec = {
			Vec3(20.0f, 0.25f, 20.0f),
			Vec3(-20.0f, 0.25f, -20.0f),
			Vec3(20.0f, 0.25f, -20.0f),
			Vec3(-20.0f, 0.25f, 20.0f)
		};
		//�I�u�W�F�N�g�̍쐬
		for (auto v : Vec) {
			AddGameObject<EnemyObject>(v);
		}
	}

	void GameStage::CreateTarget() {
		AddGameObject<TargetObject>(Vec3(5.0f, 20.0f, 5.0f),
			Vec3(0.0f, 0.0f, 0.0f),
			Vec3(0.0f, 10.0f, 45.0f));
	}
	void GameStage::OnCreate() {
		try {
			//�r���[�ƃ��C�g�̍쐬
			CreateViewLight();
			//�v���[�g�̍쐬
			CreatePlate();
			//�v���C���[�̍쐬
			CreatePlayer();
			//�G�̍쐬
			CreateEnemy();
			//�^�[�Q�b�g�̍쐬
			CreateTarget();
		}
		catch (...) {
			throw;
		}
	}

}
//end basecross
