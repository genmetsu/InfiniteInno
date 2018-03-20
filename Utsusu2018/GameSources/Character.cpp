/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross{

	//--------------------------------------------------------------------------------------
	//	class FixedBox : public GameObject;
	//	用途: 固定のボックス
	//--------------------------------------------------------------------------------------
	//構築と破棄
	FixedBox::FixedBox(const shared_ptr<Stage>& StagePtr,
		const Vec3& Scale,
		const Vec3& Rotation,
		const Vec3& Position
	) :
		GameObject(StagePtr),
		m_Scale(Scale),
		m_Rotation(Rotation),
		m_Position(Position)
	{
	}
	FixedBox::~FixedBox() {}

	//初期化
	void FixedBox::OnCreate() {
		auto PtrTransform = GetComponent<Transform>();

		PtrTransform->SetScale(m_Scale);
		PtrTransform->SetRotation(m_Rotation);
		PtrTransform->SetPosition(m_Position);

		//衝突判定
		auto PtrObb = AddComponent<CollisionObb>();
		PtrObb->SetFixed(true);

		//影をつける
		auto ShadowPtr = AddComponent<Shadowmap>();
		ShadowPtr->SetMeshResource(L"DEFAULT_CUBE");

		auto PtrDraw = AddComponent<BcPNTStaticDraw>();
		PtrDraw->SetFogEnabled(false);
		PtrDraw->SetMeshResource(L"DEFAULT_CUBE");
		PtrDraw->SetOwnShadowActive(true);
		PtrDraw->SetTextureResource(L"GRASS_TX");
	}

	//--------------------------------------------------------------------------------------
	//	class TargetObject : public GameObject;
	//	用途: 感染したキャラクターの目標となる建物
	//--------------------------------------------------------------------------------------
	//構築と破棄
	TargetObject::TargetObject(const shared_ptr<Stage>& StagePtr,
		const Vec3& Scale,
		const Vec3& Rotation,
		const Vec3& Position
	) :
		GameObject(StagePtr),
		m_Scale(Scale),
		m_Rotation(Rotation),
		m_Position(Position)
	{
	}
	TargetObject::~TargetObject() {}

	//初期化
	void TargetObject::OnCreate() {
		auto PtrTransform = GetComponent<Transform>();

		PtrTransform->SetScale(m_Scale);
		PtrTransform->SetRotation(m_Rotation);
		PtrTransform->SetPosition(m_Position);

		//衝突判定
		auto PtrObb = AddComponent<CollisionObb>();
		PtrObb->SetFixed(true);

		//影をつける
		auto ShadowPtr = AddComponent<Shadowmap>();
		ShadowPtr->SetMeshResource(L"DEFAULT_CUBE");

		auto PtrDraw = AddComponent<BcPNTStaticDraw>();
		PtrDraw->SetFogEnabled(false);
		PtrDraw->SetMeshResource(L"DEFAULT_CUBE");
		PtrDraw->SetOwnShadowActive(true);
		PtrDraw->SetTextureResource(L"WALL_TX");
	}

	void TargetObject::OnUpdate() {

	}

	//--------------------------------------------------------------------------------------
	//	class EnemyObject : public GameObject;
	//	用途: 一般的な敵
	//--------------------------------------------------------------------------------------
	//構築と破棄
	EnemyObject::EnemyObject(const shared_ptr<Stage>& StagePtr, const Vec3& StartPos) :
		GameObject(StagePtr),
		m_StartPos(StartPos)
	{
		m_OppositionSpeed = 3.0f;
		m_InfectedSpeed = 2.0f;
		m_SearchDistance = 4.0f;
		m_InfectionLength = 3.0f;
		m_InfectedPercent = 0.0f;
	}
	EnemyObject::~EnemyObject() {}
	//初期化
	void EnemyObject::OnCreate() {
		auto PtrTransform = GetComponent<Transform>();
		PtrTransform->SetPosition(m_StartPos);
		PtrTransform->SetScale(0.5f, 0.5f, 0.5f);
		PtrTransform->SetRotation(0.0f, 0.0f, 0.0f);
		//操舵系のコンポーネントをつける場合はRigidbodyをつける
		auto PtrRegid = AddComponent<Rigidbody>();
		//Sphereの衝突判定をつける
		auto CollPtr = AddComponent<CollisionSphere>();
		CollPtr->SetFixed(true);
		//影をつける
		auto ShadowPtr = AddComponent<Shadowmap>();
		ShadowPtr->SetMeshResource(L"DEFAULT_SPHERE");
		auto PtrDraw = AddComponent<BcPNTStaticDraw>();
		PtrDraw->SetFogEnabled(false);
		PtrDraw->SetMeshResource(L"DEFAULT_SPHERE");
		//PtrDraw->SetTextureResource(L"WALL_TX");

		m_SpeedMax = 0.4f;
		m_SpeedMin = 0.1f;
		m_RadiusMax = 3.0f;
		m_RadiusMin = 2.0f;

		//m_SpeedMaxが最大値と勘違する。実際出てくる数値はm_SpeedMinと足した数値なので違う

		m_Speed = Util::RandZeroToOne() * m_SpeedMax + m_SpeedMin;
		m_Radian = 0;
		m_Radius = Util::RandZeroToOne() * m_RadiusMax + m_RadiusMin;

		//Enemyというグループに組み込む
		auto Group = GetStage()->GetSharedObjectGroup(L"Enemy");
		Group->IntoGroup(GetThis<EnemyObject>());

		//ステートマシンの構築
		m_StateMachine.reset(new LayeredStateMachine<EnemyObject>(GetThis<EnemyObject>()));
		//最初のステートをPlayerDefaultにリセット
		m_StateMachine->Reset(EnemyDefaultState::Instance());

		AddTag(L"Enemy");
	}
	//更新処理
	void EnemyObject::OnUpdate() {
		//ステートマシン更新
		m_StateMachine->Update();
		//感染率で色を変える
		ColorChangeByInfection();
		//まわりに感染させる処理
		Infect();
	}

	void EnemyObject::CheckInfection() {
		if (m_InfectedPercent >= 100.0f) {
			m_StateMachine->Push(EnemyInfectedState::Instance());
			return;
		}
	}

	void EnemyObject::Move(){
		auto PtrTrans = GetComponent<Transform>();
		Vec3 Pos = PtrTrans->GetPosition();

		Pos.x = m_StartPos.x + cosf(m_Radian) * m_Radius;
		Pos.z = m_StartPos.z  + sinf(m_Radian) * m_Radius;
		m_Radian += m_Speed * 3.1415f / 180.0f;

		PtrTrans->SetPosition(Pos);
	}

	void EnemyObject::ToTargetPoint() {
		auto PtrPoint = GetStage()->GetSharedGameObject<FollowerTargetPoint>(L"FollowerTargetPoint");
		if (PtrPoint->GetDrawActive()) {
			Vec3 PointPos = PtrPoint->GetComponent<Transform>()->GetWorldPosition();
			Vec3 MyPos = GetComponent<Transform>()->GetPosition();
			Vec3 MoveVec = PointPos - MyPos;
			MoveVec.y = 0.0f;
			MoveVec.normalize();
			auto PtrRigid = GetComponent<Rigidbody>();
			PtrRigid->SetVelocity(MoveVec * m_InfectedSpeed);
		}
		else {
			ToInfectMove();
		}
	}

	//感染率が100未満の一番近い敵に近づく処理
	void EnemyObject::ToInfectMove() {
		float distance = 0.0f;
		Vec3 MoveVec = Vec3(0);
		auto& GroupVec = GetStage()->GetSharedObjectGroup(L"Enemy")->GetGroupVector();
		for (auto& v : GroupVec) {
			auto PtrEnemy = dynamic_pointer_cast<EnemyObject>(v.lock());
			if (PtrEnemy->GetInfectedPercent() < 100.0f) {
				float NowDis = (PtrEnemy->GetComponent<Transform>()->GetPosition() - GetComponent<Transform>()->GetPosition()).length();
				//最初の初期化
				if (distance == 0.0f) {
					distance = NowDis;
					MoveVec = PtrEnemy->GetComponent<Transform>()->GetPosition() - GetComponent<Transform>()->GetPosition();
					MoveVec.normalize();
				}
				//最短距離を更新する
				else if (distance > NowDis) {
					distance = NowDis;
					MoveVec = PtrEnemy->GetComponent<Transform>()->GetPosition() - GetComponent<Transform>()->GetPosition();
					MoveVec.normalize();
				}
			}
		}
		auto PtrRigid = GetComponent<Rigidbody>();
		PtrRigid->SetVelocity(MoveVec * m_InfectedSpeed);
	}

	//プレイヤーに向かう処理
	void EnemyObject::ToPlayerMove() {
		//距離を測る
		auto PtrPlayer = GetStage()->GetSharedGameObject<Player>(L"Player");
		Vec3 PlayerPos = PtrPlayer->GetComponent<Transform>()->GetPosition();
		Vec3 MyPos = GetComponent<Transform>()->GetPosition();
		float distance = (PlayerPos - MyPos).length();
		//距離が設定未満ならプレイヤーに向かう
		if (distance < m_SearchDistance) {
			Vec3 MoveVec = PlayerPos - MyPos;
			MoveVec.normalize();
			auto PtrRigid = GetComponent<Rigidbody>();
			PtrRigid->SetVelocity(MoveVec);
		}
	}

	void EnemyObject::ColorChangeByInfection() {
		auto PtrDraw = GetComponent<BcPNTStaticDraw>();
		PtrDraw->SetDiffuse(Col4(m_InfectedPercent/100.0f, 0, m_InfectedPercent/100.0f, 1.0f));
	}

	void EnemyObject::Infect() {
		if (m_InfectedPercent > 0.0f) {
			//エフェクト放出
			auto PtrSpark = GetStage()->GetSharedGameObject<InfectParticle>(L"InfectParticle", false);
			if (PtrSpark) {
				PtrSpark->InsertInfect(GetComponent<Transform>()->GetPosition(), m_InfectedPercent / 100.0f * m_InfectionLength);
			}

			//接近した相手の感染率を上げる
			auto PtrTrans = GetComponent<Transform>();
			Vec3 MyPos = PtrTrans->GetPosition();
			auto& EnemyGroup = GetStage()->GetSharedObjectGroup(L"Enemy");
			for (auto& v : EnemyGroup->GetGroupVector()) {
				//距離を取る
				Vec3 EnemyPos = v.lock()->GetComponent<Transform>()->GetPosition();
				float length = (EnemyPos - MyPos).length();
				if (length <= m_InfectedPercent / 100.0f * m_InfectionLength) {
					auto PtrEnemy = dynamic_pointer_cast<EnemyObject>(v.lock());
					float NowPercent = PtrEnemy->GetInfectedPercent();
					float NewPercent = NowPercent + 0.1f;
					if (NewPercent > 100.0f) {
						NewPercent = 100.0f;
					}
					PtrEnemy->SetInfectedPercent(NewPercent);
				}
			}
			auto PtrTarget = GetStage()->GetSharedGameObject<TargetObject>(L"TargetObject");
			Vec3 TargetPos = PtrTarget->GetComponent<Transform>()->GetPosition();
			float TargetLength = (TargetPos - MyPos).length();
			if (TargetLength <= 13.0f) {
				TargetPos.y -= 0.002f;
				PtrTarget->GetComponent<Transform>()->SetPosition(TargetPos);
			}

			//自身の感染率を上げる
			if (m_InfectedPercent < 100.0f) {
				m_InfectedPercent += 0.05f;
			}
			if (m_InfectedPercent > 100.0f) {
				m_InfectedPercent = 100.0f;
			}
		}
	}

	//--------------------------------------------------------------------------------------
	///	通常ステート
	//--------------------------------------------------------------------------------------
	IMPLEMENT_SINGLETON_INSTANCE(EnemyDefaultState)
	void EnemyDefaultState::Enter(const shared_ptr<EnemyObject>& Obj) {
		
	}

	void EnemyDefaultState::Execute(const shared_ptr<EnemyObject>& Obj) {
		Obj->CheckInfection();
		Obj->Move();
	}

	void EnemyDefaultState::Exit(const shared_ptr<EnemyObject>& Obj) {
		
	}

	//--------------------------------------------------------------------------------------
	///	敵、敵対ステート
	//--------------------------------------------------------------------------------------
	IMPLEMENT_SINGLETON_INSTANCE(EnemyOppositionState)
	void EnemyOppositionState::Enter(const shared_ptr<EnemyObject>& Obj) {
		
	}

	void EnemyOppositionState::Execute(const shared_ptr<EnemyObject>& Obj) {
		Obj->ToPlayerMove();
	}

	void EnemyOppositionState::Exit(const shared_ptr<EnemyObject>& Obj) {
		
	}

	//--------------------------------------------------------------------------------------
	///	敵が感染率が100%になり味方になった時のステート
	//--------------------------------------------------------------------------------------
	IMPLEMENT_SINGLETON_INSTANCE(EnemyInfectedState)
	void EnemyInfectedState::Enter(const shared_ptr<EnemyObject>& Obj) {
		
	}

	void EnemyInfectedState::Execute(const shared_ptr<EnemyObject>& Obj) {
		Obj->ToTargetPoint();
	}

	void EnemyInfectedState::Exit(const shared_ptr<EnemyObject>& Obj) {
		
	}

}
//end basecross
