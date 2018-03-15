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
		//ステージへのゲームオブジェクトの追加
		auto Ptr = AddGameObject<GameObject>();
		auto PtrTrans = Ptr->GetComponent<Transform>();
		Quat Qt(Vec3(1.0f, 0, 0), XM_PIDIV2);
		Qt.rotationRollPitchYawFromVector(Vec3(XM_PIDIV2, 0, 0));
		PtrTrans->SetScale(100.0f, 100.0f, 1.0f);
		PtrTrans->SetQuaternion(Qt);
		PtrTrans->SetPosition(0.0f, 0.0f, 0.0f);

		auto ColPtr = Ptr->AddComponent<CollisionRect>();
		//描画コンポーネントの追加
		auto DrawComp = Ptr->AddComponent<BcPNTStaticDraw>();
		//描画コンポーネントに形状（メッシュ）を設定
		DrawComp->SetMeshResource(L"DEFAULT_SQUARE");
		DrawComp->SetFogEnabled(false);
		//自分に影が映りこむようにする
		DrawComp->SetOwnShadowActive(true);

		//描画コンポーネントテクスチャの設定
		DrawComp->SetTextureResource(L"SKY_TX");


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
		//アタックボールは50個用意する
		for (int i = 0; i < 50; i++) {
			AddGameObject<AttackBall>();
		}

		//プレーヤーの作成
		auto PlayerPtr = AddGameObject<Player>();
		//シェア配列にプレイヤーを追加
		SetSharedGameObject(L"Player", PlayerPtr);

		AddGameObject<CameraTargetPoint>();
	}

	void GameStage::CreateEnemy() {
		//配列の初期化
		vector<Vec3> Vec = {
			Vec3(20.0f, 0.25f, 20.0f),
			Vec3(-20.0f, 0.25f, -20.0f),
			Vec3(20.0f, 0.25f, -20.0f),
			Vec3(-20.0f, 0.25f, 20.0f)
		};
		//オブジェクトの作成
		for (auto v : Vec) {
			AddGameObject<EnemyObject>(v);
		}
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
		}
		catch (...) {
			throw;
		}
	}

}
//end basecross
