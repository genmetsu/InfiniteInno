/*!
@file GameStage.cpp
@brief ƒQ[ƒ€ƒXƒe[ƒWÀ‘Ì
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ƒQ[ƒ€ƒXƒe[ƒWƒNƒ‰ƒXÀ‘Ì
	//--------------------------------------------------------------------------------------
	//ƒrƒ…[‚Æƒ‰ƒCƒg‚Ìì¬
	void GameStage::CreateViewLight() {
		auto PtrView = CreateView<SingleView>();
		//ƒrƒ…[‚ÌƒJƒƒ‰‚Ìİ’è
		auto PtrLookAtCamera = ObjectFactory::Create<MyCamera>();
		PtrView->SetCamera(PtrLookAtCamera);
		PtrLookAtCamera->SetEye(Vec3(0.0f, 5.0f, -5.0f));
		PtrLookAtCamera->SetAt(Vec3(0.0f, 0.0f, 0.0f));

		PtrLookAtCamera->SetRotSpeed(3.0f);
		PtrLookAtCamera->SetLRBaseMode(false);
		PtrLookAtCamera->SetUDBaseMode(false);
		////ƒVƒ“ƒOƒ‹ƒ‰ƒCƒg‚Ìì¬
		//auto PtrSingleLight = CreateLight<SingleLight>();
		////ƒ‰ƒCƒg‚Ìİ’è
		//PtrSingleLight->GetLight().SetPositionToDirectional(-0.001f, 1.0f, -0.001f);

		//ƒ}ƒ‹ƒ`ƒ‰ƒCƒg‚Ìì¬
		auto PtrMultiLight = CreateLight<MultiLight>();
		//ƒfƒtƒHƒ‹ƒg‚Ìƒ‰ƒCƒeƒBƒ“ƒO‚ğw’è
		PtrMultiLight->SetDefaultLighting();
		PtrMultiLight->SetAmbientLightColor(Col4(0.08f, 0.04f, 0.0f, 1));
	}

	//ƒvƒŒ[ƒg‚Ìì¬
	void GameStage::CreatePlate() {
		//ƒXƒe[ƒW‚Ö‚ÌƒQ[ƒ€ƒIƒuƒWƒFƒNƒg‚Ì’Ç‰Á
		auto Ptr = AddGameObject<GameObject>();
		auto PtrTrans = Ptr->GetComponent<Transform>();
		Quat Qt(Vec3(1.0f, 0, 0), XM_PIDIV2);
		Qt.rotationRollPitchYawFromVector(Vec3(XM_PIDIV2, 0, 0));
		PtrTrans->SetScale(100.0f, 100.0f, 1.0f);
		PtrTrans->SetQuaternion(Qt);
		PtrTrans->SetPosition(0.0f, 0.0f, 0.0f);

		auto ColPtr = Ptr->AddComponent<CollisionRect>();
		//•`‰æƒRƒ“ƒ|[ƒlƒ“ƒg‚Ì’Ç‰Á
		auto DrawComp = Ptr->AddComponent<BcPNTStaticDraw>();
		//•`‰æƒRƒ“ƒ|[ƒlƒ“ƒg‚ÉŒ`óiƒƒbƒVƒ…j‚ğİ’è
		DrawComp->SetMeshResource(L"DEFAULT_SQUARE");
		DrawComp->SetFogEnabled(false);
		//©•ª‚É‰e‚ª‰f‚è‚±‚Ş‚æ‚¤‚É‚·‚é
		DrawComp->SetOwnShadowActive(true);

		//•`‰æƒRƒ“ƒ|[ƒlƒ“ƒgƒeƒNƒXƒ`ƒƒ‚Ìİ’è
		DrawComp->SetTextureResource(L"SKY_TX");


		//•Ç‚Ìì¬
		//”z—ñ‚Ì‰Šú‰»
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
		//ƒIƒuƒWƒFƒNƒg‚Ìì¬
		for (auto v : Vec) {
			AddGameObject<FixedBox>(v[0], v[1], v[2]);
		}

	}

	//ƒvƒŒƒCƒ„[‚Ìì¬
	void GameStage::CreatePlayer() {

		CreateSharedObjectGroup(L"AttackBall");
		//ƒAƒ^ƒbƒNƒ{[ƒ‹‚Í50ŒÂ—pˆÓ‚·‚é
		for (int i = 0; i < 50; i++) {
			AddGameObject<AttackBall>();
		}

		//ƒvƒŒ[ƒ„[‚Ìì¬
		auto PlayerPtr = AddGameObject<Player>();
		//ƒVƒFƒA”z—ñ‚ÉƒvƒŒƒCƒ„[‚ğ’Ç‰Á
		SetSharedGameObject(L"Player", PlayerPtr);

		AddGameObject<CameraTargetPoint>();
	}

	void GameStage::CreateEnemy() {
		//”z—ñ‚Ì‰Šú‰»
		vector<Vec3> Vec = {
			Vec3(20.0f, 0.25f, 20.0f),
			Vec3(-20.0f, 0.25f, -20.0f),
			Vec3(20.0f, 0.25f, -20.0f),
			Vec3(-20.0f, 0.25f, 20.0f)
		};
		//ƒIƒuƒWƒFƒNƒg‚Ìì¬
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
			//ƒrƒ…[‚Æƒ‰ƒCƒg‚Ìì¬
			CreateViewLight();
			//ƒvƒŒ[ƒg‚Ìì¬
			CreatePlate();
			//ƒvƒŒƒCƒ„[‚Ìì¬
			CreatePlayer();
			//“G‚Ìì¬
			CreateEnemy();
			//ï¿½^ï¿½[ï¿½Qï¿½bï¿½gï¿½Ìì¬
			CreateTarget();
		}
		catch (...) {
			throw;
		}
	}

}
//end basecross
