/*!
@file Player.cpp
@brief プレイヤーなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross{

	//--------------------------------------------------------------------------------------
	//	class AttackBall : public GameObject;
	//	用途: 飛んでいくボール
	//--------------------------------------------------------------------------------------
	//構築と破棄
	AttackBall::AttackBall(const shared_ptr<Stage>& StagePtr) :
		GameObject(StagePtr) {}

	AttackBall::~AttackBall() {}

	void AttackBall::Weakup(const Vec3& Position, const Vec3& Velocity) {
		auto PtrTransform = GetComponent<Transform>();
		PtrTransform->SetScale(0.1f, 0.1f, 0.1f);
		PtrTransform->SetRotation(0, 0, 0);
		PtrTransform->ResetPosition(Position);
		auto PtrRedid = GetComponent<Rigidbody>();
		PtrRedid->SetVelocity(Velocity);
		SetDrawActive(true);
		SetUpdateActive(true);
	}


	//初期化
	void AttackBall::OnCreate() {
		auto PtrTransform = GetComponent<Transform>();

		PtrTransform->SetScale(0.1f, 0.1f, 0.1f);
		PtrTransform->SetRotation(0, 0, 0);
		PtrTransform->SetPosition(0, 0, 0);

		//Rigidbodyをつける
		auto PtrRedid = AddComponent<Rigidbody>();

		//衝突判定をつける
		auto PtrCol = AddComponent<CollisionSphere>();

		//影をつける
		auto ShadowPtr = AddComponent<Shadowmap>();
		ShadowPtr->SetMeshResource(L"DEFAULT_SPHERE");

		auto PtrDraw = AddComponent<BcPNTStaticDraw>();
		PtrDraw->SetMeshResource(L"DEFAULT_SPHERE");
		PtrDraw->SetDiffuse(Col4(0, 0, 1, 1));

		auto Group = GetStage()->GetSharedObjectGroup(L"AttackBall");
		Group->IntoGroup(GetThis<AttackBall>());

		//最初は無効にしておく
		SetDrawActive(false);
		SetUpdateActive(false);

	}

	void AttackBall::OnUpdate() {
		
	}

	void AttackBall::OnCollision(vector<shared_ptr<GameObject>>& OtherVec) {
		auto PtrTransform = GetComponent<Transform>();
		PtrTransform->SetScale(0.1f, 0.1f, 0.1f);
		PtrTransform->SetRotation(0, 0, 0);
		PtrTransform->SetPosition(0, 0, 0);
		SetDrawActive(false);
		SetUpdateActive(false);
	}

	//--------------------------------------------------------------------------------------
	//	class Player : public GameObject;
	//	用途: プレイヤー
	//--------------------------------------------------------------------------------------
	//構築と破棄
	Player::Player(const shared_ptr<Stage>& StagePtr) :
		GameObject(StagePtr)
	{}

	//初期化
	void Player::OnCreate() {

		//初期位置などの設定
		auto Ptr = AddComponent<Transform>();
		Ptr->SetScale(0.5f, 0.5f, 0.5f);	//直径25センチの球体
		Ptr->SetRotation(0.0f, 0.0f, 0.0f);
		Ptr->SetPosition(0, 0.125f, 0);

		//Rigidbodyをつける
		auto PtrRedid = AddComponent<Rigidbody>();
		//衝突判定をつける
		auto PtrCol = AddComponent<CollisionSphere>();
		PtrCol->SetIsHitAction(IsHitAction::Auto);
		//文字列をつける
		auto PtrString = AddComponent<StringSprite>();
		PtrString->SetText(L"");
		PtrString->SetTextRect(Rect2D<float>(16.0f, 16.0f, 640.0f, 480.0f));

		//影をつける（シャドウマップを描画する）
		auto ShadowPtr = AddComponent<Shadowmap>();
		//影の形（メッシュ）を設定
		ShadowPtr->SetMeshResource(L"DEFAULT_SPHERE");

		//描画コンポーネントの設定
		auto PtrDraw = AddComponent<BcPNTStaticDraw>();
		//描画するメッシュを設定
		PtrDraw->SetMeshResource(L"DEFAULT_SPHERE");
		//描画するテクスチャを設定
		PtrDraw->SetTextureResource(L"TRACE_TX");
		//透明処理
		SetAlphaActive(true);

		//サウンドを登録.
		auto pMultiSoundEffect = AddComponent<MultiSoundEffect>();
		pMultiSoundEffect->AddAudioResource(L"cursor");

		//カメラを得る
		auto PtrCamera = dynamic_pointer_cast<MyCamera>(OnGetDrawCamera());
		if (PtrCamera) {
			//MyCameraで注目するオブジェクト（プレイヤー）の設定
			PtrCamera->SetTargetObject(GetThis<GameObject>());
			PtrCamera->SetTargetToAt(Vec3(0, 0.75f, 0));
		}

		//最初はAボタンはジャンプ
		m_PlayerAction = PlayerAction::Attack;
		//ステートマシンの構築
		m_StateMachine.reset(new LayeredStateMachine<Player>(GetThis<Player>()));
		//最初のステートをPlayerDefaultにリセット
		m_StateMachine->Reset(PlayerDefaultState::Instance());
	}

	//更新
	void Player::OnUpdate() {
		//コントローラチェックして入力があればコマンド呼び出し
		m_InputHandler.PushHandle(GetThis<Player>());
		//ステートマシン更新
		m_StateMachine->Update();
	}

	//後更新
	void Player::OnUpdate2() {
		//文字列の表示
		DrawStrings();
	}


	void Player::OnCollision(vector<shared_ptr<GameObject>>& OtherVec) {

		auto PtrBehavior = GetBehavior<PlayerBehavior>();
		shared_ptr<GameObject> v;
		if (PtrBehavior->OnHitObjMoveBox(OtherVec, v)) {
			GetStateMachine()->Reset(PlayerOnMoveboxState::Instance());
			GetComponent<Transform>()->SetParent(v);
			return;
		}

		if (GetStateMachine()->GetTopState() == PlayerJumpState::Instance()) {
			//現在がジャンプステートか移動ボックスステートならPlayerDefaultにリセット
			GetStateMachine()->Reset(PlayerDefaultState::Instance());
		}
	}

	//Rボタン
	void  Player::OnPushRightSoulder() {
		if (GetStateMachine()->GetTopState() == PlayerDefaultState::Instance() ||
			GetStateMachine()->GetTopState() == PlayerOnMoveboxState::Instance()) {
			switch (m_PlayerAction) {
			case PlayerAction::Jump:
				GetStateMachine()->Push(PlayerJumpState::Instance());
				break;
			case PlayerAction::Attack:
				GetStateMachine()->Push(PlayerAttackState::Instance());
				break;
			}
		}
	}

	//Bボタン
	void  Player::OnPushB() {
		switch (m_PlayerAction) {
		case PlayerAction::Jump:
			m_PlayerAction = PlayerAction::Attack;
			break;
		case PlayerAction::Attack:
			m_PlayerAction = PlayerAction::Jump;
			break;
		}
	}



	//文字列の表示
	void Player::DrawStrings() {

		//文字列表示
		auto fps = App::GetApp()->GetStepTimer().GetFramesPerSecond();
		wstring FPS(L"FPS: ");
		FPS += Util::UintToWStr(fps);
		FPS += L"\nElapsedTime: ";
		float ElapsedTime = App::GetApp()->GetElapsedTime();
		FPS += Util::FloatToWStr(ElapsedTime);
		FPS += L"\n";

		wstring StateStr(L"State: ");
		StateStr += GetStateMachine()->GetTopState()->GetStateName() + L"\n";

		wstring HitObjectStr(L"HitObject: ");
		if (GetComponent<Collision>()->GetHitObjectVec().size() > 0) {
			for (auto&v : GetComponent<Collision>()->GetHitObjectVec()) {
				HitObjectStr += Util::UintToWStr((UINT)v.get()) + L",";
			}
			HitObjectStr += L"\n";
		}
		else {
			HitObjectStr += L"NULL\n";
		}
		wstring str = FPS + StateStr + HitObjectStr;
		//文字列をつける
		auto PtrString = GetComponent<StringSprite>();
		PtrString->SetFontColor(Col4(0, 0, 0, 1));
		PtrString->SetText(str);
	}


	//--------------------------------------------------------------------------------------
	///	通常ステート
	//--------------------------------------------------------------------------------------

	IMPLEMENT_SINGLETON_INSTANCE(PlayerDefaultState)

		void PlayerDefaultState::Enter(const shared_ptr<Player>& Obj) {
		//何もしない
	}

	void PlayerDefaultState::Execute(const shared_ptr<Player>& Obj) {
		auto PtrBehavior = Obj->GetBehavior<PlayerBehavior>();
		PtrBehavior->MovePlayer();
		auto PtrGrav = Obj->GetBehavior<Gravity>();
		PtrGrav->Execute();
	}

	void PlayerDefaultState::Exit(const shared_ptr<Player>& Obj) {
		//何もしない
	}

	//--------------------------------------------------------------------------------------
	///	MoveBoxに乗っているステート
	//--------------------------------------------------------------------------------------
	IMPLEMENT_SINGLETON_INSTANCE(PlayerOnMoveboxState)
		void PlayerOnMoveboxState::Enter(const shared_ptr<Player>& Obj) {
		Obj->GetComponent<CollisionSphere>()->SetUpdateActive(false);
	}

	void PlayerOnMoveboxState::Execute(const shared_ptr<Player>& Obj) {
		auto PtrBehavior = Obj->GetBehavior<PlayerBehavior>();
		PtrBehavior->MovePlayer();
		if (!PtrBehavior->OnMoveBox()) {
			Obj->GetStateMachine()->Push(PlayerJumpState::Instance());
		}
	}

	void PlayerOnMoveboxState::Sleep(const shared_ptr<Player>& Obj) {
		Obj->GetComponent<CollisionSphere>()->SetUpdateActive(true);
	}

	void PlayerOnMoveboxState::Exit(const shared_ptr<Player>& Obj) {
		Obj->GetComponent<CollisionSphere>()->SetUpdateActive(true);
		Obj->GetComponent<Transform>()->SetParent(nullptr);
	}

	//--------------------------------------------------------------------------------------
	///	ジャンプステート
	//--------------------------------------------------------------------------------------
	IMPLEMENT_SINGLETON_INSTANCE(PlayerJumpState)

		void PlayerJumpState::Enter(const shared_ptr<Player>& Obj) {
		auto PtrGrav = Obj->GetBehavior<Gravity>();
		PtrGrav->StartJump(Vec3(0, 4.0f, 0));
		auto pMultiSoundEffect = Obj->GetComponent<MultiSoundEffect>();
		pMultiSoundEffect->Start(L"cursor", 0, 0.5f);
	}

	void PlayerJumpState::Execute(const shared_ptr<Player>& Obj) {
		//ジャンプ中も方向変更可能
		auto PtrBehavior = Obj->GetBehavior<PlayerBehavior>();
		PtrBehavior->MovePlayer();
		auto PtrGrav = Obj->GetBehavior<Gravity>();
		PtrGrav->Execute();
	}

	void PlayerJumpState::Exit(const shared_ptr<Player>& Obj) {
		//何もしない
	}

	//--------------------------------------------------------------------------------------
	///	アタックステート
	//--------------------------------------------------------------------------------------
	IMPLEMENT_SINGLETON_INSTANCE(PlayerAttackState)

		void PlayerAttackState::Enter(const shared_ptr<Player>& Obj) {
		auto PtrBehavior = Obj->GetBehavior<PlayerBehavior>();
		PtrBehavior->FireAttackBall();
	}

	void PlayerAttackState::Execute(const shared_ptr<Player>& Obj) {
		//すぐにステートを戻す
		Obj->GetStateMachine()->Pop();
	}

	void PlayerAttackState::Exit(const shared_ptr<Player>& Obj) {
		//何もしない
	}


	//--------------------------------------------------------------------------------------
	//	class CameraTargetPoint : public GameObject;
	//	用途: カメラの注目点
	//--------------------------------------------------------------------------------------
	//構築と破棄
	CameraTargetPoint::CameraTargetPoint(const shared_ptr<Stage>& StagePtr) :
		GameObject(StagePtr)
	{}

	//初期化
	void CameraTargetPoint::OnCreate() {

		//初期位置などの設定
		auto Ptr = AddComponent<Transform>();
		Ptr->SetScale(0.1f, 0.1f, 0.1f);
		Ptr->SetRotation(0.0f, 0.0f, 0.0f);
		Ptr->SetPosition(0.0f, 0, 1.0f);

		//描画コンポーネントの設定
		auto PtrDraw = AddComponent<BcPNTStaticDraw>();
		//描画するメッシュを設定
		PtrDraw->SetMeshResource(L"DEFAULT_SPHERE");
		//描画するテクスチャを設定
		PtrDraw->SetTextureResource(L"TRACE_TX");
		//透明処理
		SetAlphaActive(true);

		auto PtrPlayer = GetStage()->GetSharedGameObject<Player>(L"Player");
		GetComponent<Transform>()->SetParent(PtrPlayer);
	}

	//更新
	void CameraTargetPoint::OnUpdate() {
	}

	//後更新
	void CameraTargetPoint::OnUpdate2() {
	}

}
//end basecross

