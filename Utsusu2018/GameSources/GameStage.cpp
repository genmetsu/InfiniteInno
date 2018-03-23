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

		//�v���[�g�p�̃��b�V�����쐬
		vector<VertexPositionNormalTexture> vertices;
		vector<VertexPositionNormalTangentTexture> new_pntnt_vertices;
		vector<uint16_t> indices;
		MeshUtill::CreateSquare(1.0f, vertices, indices);
		for (size_t i = 0; i < vertices.size(); i++) {
			VertexPositionNormalTangentTexture new_pntnt_v;
			new_pntnt_v.position = vertices[i].position;
			new_pntnt_v.normal = vertices[i].normal;
			new_pntnt_v.textureCoordinate = vertices[i].textureCoordinate;
			if (vertices[i].textureCoordinate.x == 1.0f) {
				new_pntnt_v.textureCoordinate.x = 20.0f;
			}
			if (vertices[i].textureCoordinate.y == 1.0f) {
				new_pntnt_v.textureCoordinate.y = 20.0f;
			}
			Vec3 n = cross((Vec3)new_pntnt_v.normal, Vec3(0, 1, 0));
			new_pntnt_v.tangent = Vec4(n, 0.0f);
			new_pntnt_v.tangent.w = 0.0f;
			new_pntnt_vertices.push_back(new_pntnt_v);
		}
		MeshUtill::SetNormalTangent(new_pntnt_vertices);
		App::GetApp()->RegisterResource(L"MY_PNTnT_SQUARE", MeshResource::CreateMeshResource(new_pntnt_vertices, indices, false));

		//�X�e�[�W�ւ̃Q�[���I�u�W�F�N�g�̒ǉ�
		auto Ptr = AddGameObject<GameObject>();
		auto PtrTrans = Ptr->GetComponent<Transform>();
		Quat Qt;
		Qt.rotationRollPitchYawFromVector(Vec3(XM_PIDIV2, 0, 0));
		PtrTrans->SetScale(100.0f, 100.0f, 1.0f);
		PtrTrans->SetQuaternion(Qt);
		PtrTrans->SetPosition(0.0f, 0.0f, 0.0f);
		auto ColPtr = Ptr->AddComponent<CollisionRect>();
		//�`��R���|�[�l���g
		auto DrawComp = Ptr->AddComponent<BcPNTnTStaticDraw>();
		DrawComp->SetMeshResource(L"MY_PNTnT_SQUARE");
		DrawComp->SetTextureResource(L"GRASS_TX");
		DrawComp->SetNormalMapTextureResource(L"GRASS_TX");
		//���b�v���[�h
		DrawComp->SetSamplerState(SamplerState::LinearWrap);
		//�X�y�L�����[�Ȃ�
		DrawComp->DisableSpecular();
		//�t�H�O�͂��߂�
		DrawComp->SetFogEnabled(true);
		DrawComp->SetFogColor(Col4(0.3f, 0.3f, 0.3f, 1.0f));
		DrawComp->SetFogStart(-10.0f);
		DrawComp->SetFogEnd(-30.0f);
		//�����ɉe���f�肱�ނ悤�ɂ���
		DrawComp->SetOwnShadowActive(true);


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
		//�A�^�b�N�{�[����10�p�ӂ���
		for (int i = 0; i < 10; i++) {
			AddGameObject<AttackBall>();
		}

		//�v���[���[�̍쐬
		auto PlayerPtr = AddGameObject<Player>();
		//�V�F�A�z��Ƀv���C���[��ǉ�
		SetSharedGameObject(L"Player", PlayerPtr);

		AddGameObject<CameraTargetPoint>();

		auto PtrPoint = AddGameObject<FollowerTargetPoint>();
		SetSharedGameObject(L"FollowerTargetPoint", PtrPoint);

		AddGameObject<HPGaugeSprite>(true, Vec3(-600.0f,300.0f, 0.0f), L"RED_TX");
	}

	void GameStage::CreateEnemy() {

		CreateSharedObjectGroup(L"Enemy");
		CreateSharedObjectGroup(L"Infected");

		//�z��̏�����
		vector<Vec3> Vec = {
			Vec3(10.0f, 0.25f, 10.0f),
			Vec3(-10.0f, 0.25f, -10.0f),
			Vec3(14.0f, 0.25f, -7.0f),
			Vec3(18.0f, 0.25f, -2.0f),
			Vec3(-4.0f, 0.25f, -10.0f),
			Vec3(-14.0f, 0.25f, -1.0f),
			Vec3(-10.0f, 0.25f, 10.0f)
		};
		//�I�u�W�F�N�g�̍쐬
		for (int i = 0; i < 20;i++) {
			AddGameObject<EnemyObject>(Vec3((Util::RandZeroToOne() - 0.5f) * 35.0f, 0.25f, (Util::RandZeroToOne() - 0.5f) * 35.0f));
		}
	}

	void GameStage::CreateTarget() {
		auto PtrTarget = AddGameObject<TargetObject>(Vec3(5.0f, 20.0f, 5.0f),
			Vec3(0.0f, 0.0f, 0.0f),
			Vec3(0.0f, 10.0f, 45.0f));
		SetSharedGameObject(L"TargetObject", PtrTarget);
	}

	void GameStage::CreateParticle() {
		auto MultiSparkPtr = AddGameObject<MultiSpark>();
		SetSharedGameObject(L"MultiSpark", MultiSparkPtr);
		auto InfectParticlePtr = AddGameObject<InfectParticle>();
		SetSharedGameObject(L"InfectParticle", InfectParticlePtr);
	}

	//�񕜃A�C�e��
	void GameStage::CreateCureObject() {

		auto PtrCureObject = AddGameObject<CureObject>(Vec3(0, 0, 2.0f));
		SetSharedGameObject(L"CureObject", PtrCureObject);

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
			//�񕜂���A�C�e���̍쐬
			CreateCureObject();

			CreateParticle();
		}
		catch (...) {
			throw;
		}
	}

}
//end basecross
