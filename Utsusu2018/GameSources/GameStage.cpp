/*!
@file GameStage.cpp
@brief ゲームステージ実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス実体
	//--------------------------------------------------------------------------------------
	//ビューとライトの作成
	void GameStage::CreateViewLight() {
		auto PtrView = CreateView<SingleView>();
		//ビューのカメラの設定
		auto PtrLookAtCamera = ObjectFactory::Create<MyCamera>();
		PtrView->SetCamera(PtrLookAtCamera);
		PtrLookAtCamera->SetEye(Vec3(0.0f, 5.0f, -5.0f));
		PtrLookAtCamera->SetAt(Vec3(0.0f, 0.0f, 0.0f));

		PtrLookAtCamera->SetRotSpeed(3.0f);
		PtrLookAtCamera->SetLRBaseMode(false);
		PtrLookAtCamera->SetUDBaseMode(false);
		////シングルライトの作成
		//auto PtrSingleLight = CreateLight<SingleLight>();
		////ライトの設定
		//PtrSingleLight->GetLight().SetPositionToDirectional(-0.001f, 1.0f, -0.001f);

		//マルチライトの作成
		auto PtrMultiLight = CreateLight<MultiLight>();
		//デフォルトのライティングを指定
		PtrMultiLight->SetDefaultLighting();
		PtrMultiLight->SetAmbientLightColor(Col4(0.08f, 0.04f, 0.0f, 1));
	}

	//プレートの作成
	void GameStage::CreatePlate() {

		//プレート用のメッシュを作成
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

		//ステージへのゲームオブジェクトの追加
		auto Ptr = AddGameObject<GameObject>();
		auto PtrTrans = Ptr->GetComponent<Transform>();
		Quat Qt;
		Qt.rotationRollPitchYawFromVector(Vec3(XM_PIDIV2, 0, 0));
		PtrTrans->SetScale(100.0f, 100.0f, 1.0f);
		PtrTrans->SetQuaternion(Qt);
		PtrTrans->SetPosition(0.0f, 0.0f, 0.0f);
		auto ColPtr = Ptr->AddComponent<CollisionRect>();
		//描画コンポーネント
		auto DrawComp = Ptr->AddComponent<BcPNTnTStaticDraw>();
		DrawComp->SetMeshResource(L"MY_PNTnT_SQUARE");
		DrawComp->SetTextureResource(L"GRASS_TX");
		DrawComp->SetNormalMapTextureResource(L"GRASS_TX");
		//ラップモード
		DrawComp->SetSamplerState(SamplerState::LinearWrap);
		//スペキュラーなし
		DrawComp->DisableSpecular();
		//フォグはきつめに
		DrawComp->SetFogEnabled(true);
		DrawComp->SetFogColor(Col4(0.3f, 0.3f, 0.3f, 1.0f));
		DrawComp->SetFogStart(-10.0f);
		DrawComp->SetFogEnd(-30.0f);
		//自分に影が映りこむようにする
		DrawComp->SetOwnShadowActive(true);


		//壁の作成
		//配列の初期化
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
		//オブジェクトの作成
		for (auto v : Vec) {
			AddGameObject<FixedBox>(v[0], v[1], v[2]);
		}

	}

	//プレイヤーの作成
	void GameStage::CreatePlayer() {

		CreateSharedObjectGroup(L"AttackBall");
		//アタックボールは10個用意する
		for (int i = 0; i < 10; i++) {
			AddGameObject<AttackBall>();
		}

		//プレーヤーの作成
		auto PlayerPtr = AddGameObject<Player>();
		//シェア配列にプレイヤーを追加
		SetSharedGameObject(L"Player", PlayerPtr);

		AddGameObject<CameraTargetPoint>();

		auto PtrPoint = AddGameObject<FollowerTargetPoint>();
		SetSharedGameObject(L"FollowerTargetPoint", PtrPoint);

		AddGameObject<HPGaugeSprite>(true, Vec3(-600.0f,300.0f, 0.0f), L"RED_TX");
	}

	void GameStage::CreateEnemy() {

		CreateSharedObjectGroup(L"Enemy");
		CreateSharedObjectGroup(L"Infected");

		//配列の初期化
		vector<Vec3> Vec = {
			Vec3(10.0f, 0.25f, 10.0f),
			Vec3(-10.0f, 0.25f, -10.0f),
			Vec3(14.0f, 0.25f, -7.0f),
			Vec3(18.0f, 0.25f, -2.0f),
			Vec3(-4.0f, 0.25f, -10.0f),
			Vec3(-14.0f, 0.25f, -1.0f),
			Vec3(-10.0f, 0.25f, 10.0f)
		};
		//オブジェクトの作成
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

	//回復アイテム
	void GameStage::CreateCureObject() {

		auto PtrCureObject = AddGameObject<CureObject>(Vec3(0, 0, 2.0f));
		SetSharedGameObject(L"CureObject", PtrCureObject);

	}

	void GameStage::OnCreate() {
		try {
			//ビューとライトの作成
			CreateViewLight();
			//プレートの作成
			CreatePlate();
			//プレイヤーの作成
			CreatePlayer();
			//敵の作成
			CreateEnemy();
			//ターゲットの作成
			CreateTarget();
			//回復するアイテムの作成
			CreateCureObject();

			CreateParticle();
		}
		catch (...) {
			throw;
		}
	}

}
//end basecross
