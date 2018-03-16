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
		PtrDraw->SetTextureResource(L"SKY_TX");
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
		m_InfectionLength = 0.0f;
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

		m_Speed = 0.5f;
		m_Radian = 0;
		m_Radius = 5.0f;

		//Enemyというグループに組み込む
		auto Group = GetStage()->GetSharedObjectGroup(L"Enemy");
		Group->IntoGroup(GetThis<EnemyObject>());

		AddTag(L"Enemy");
	}
	//更新処理
	void EnemyObject::OnUpdate() {
		Move();
		ColorChangeByInfection();
		Infect();
	}

	void EnemyObject::Move(){
		auto PtrTrans = GetComponent<Transform>();
		Vec3 Pos = PtrTrans->GetPosition();
		
		Pos.x = m_StartPos.x + cosf(m_Radian) * m_Radius;
		Pos.z = m_StartPos.z  + sinf(m_Radian) * m_Radius;
		m_Radian += m_Speed * 3.1415f / 180;

		PtrTrans->SetPosition(Pos);
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
				PtrSpark->InsertInfect(GetComponent<Transform>()->GetPosition(), m_InfectedPercent / 100.0f);
			}

			//接近した相手の感染率を上げる
			auto& EnemyGroup = GetStage()->GetSharedObjectGroup(L"Enemy");
			for (auto& v : EnemyGroup->GetGroupVector()) {
				//距離を取る
				Vec3 EnemyPos = v.lock()->GetComponent<Transform>()->GetPosition();
				auto PtrTrans = GetComponent<Transform>();
				Vec3 MyPos = PtrTrans->GetPosition();
				float length = (EnemyPos - MyPos).length();
				if (length <= m_InfectedPercent / 100.0f) {
					auto PtrEnemy = dynamic_pointer_cast<EnemyObject>(v.lock());
					float NowPercent = PtrEnemy->GetInfectedPercent();
					float NewPercent = NowPercent + 0.1f;
					if (NewPercent > 100.0f) {
						NewPercent = 100.0f;
					}
					PtrEnemy->SetInfectedPercent(NewPercent);
				}
			}
			//自身の感染率を上げる
			if (m_InfectedPercent < 100.0f) {
				m_InfectedPercent += 0.05f;
			}
		}
	}

}
//end basecross
