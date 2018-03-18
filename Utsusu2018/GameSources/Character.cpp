/*!
@file Character.cpp
@brief �L�����N�^�[�Ȃǎ���
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross{

	//--------------------------------------------------------------------------------------
	//	class FixedBox : public GameObject;
	//	�p�r: �Œ�̃{�b�N�X
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
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

	//������
	void FixedBox::OnCreate() {
		auto PtrTransform = GetComponent<Transform>();

		PtrTransform->SetScale(m_Scale);
		PtrTransform->SetRotation(m_Rotation);
		PtrTransform->SetPosition(m_Position);

		//�Փ˔���
		auto PtrObb = AddComponent<CollisionObb>();
		PtrObb->SetFixed(true);

		//�e������
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
	//	�p�r: ���������L�����N�^�[�̖ڕW�ƂȂ錚��
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
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

	//������
	void TargetObject::OnCreate() {
		auto PtrTransform = GetComponent<Transform>();

		PtrTransform->SetScale(m_Scale);
		PtrTransform->SetRotation(m_Rotation);
		PtrTransform->SetPosition(m_Position);

		//�Փ˔���
		auto PtrObb = AddComponent<CollisionObb>();
		PtrObb->SetFixed(true);

		//�e������
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
	//	�p�r: ��ʓI�ȓG
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
	EnemyObject::EnemyObject(const shared_ptr<Stage>& StagePtr, const Vec3& StartPos) :
		GameObject(StagePtr),
		m_StartPos(StartPos)
	{
		m_InfectionLength = 3.0f;
		m_InfectedPercent = 0.0f;
	}
	EnemyObject::~EnemyObject() {}
	//������
	void EnemyObject::OnCreate() {
		auto PtrTransform = GetComponent<Transform>();
		PtrTransform->SetPosition(m_StartPos);
		PtrTransform->SetScale(0.5f, 0.5f, 0.5f);
		PtrTransform->SetRotation(0.0f, 0.0f, 0.0f);
		//���ǌn�̃R���|�[�l���g������ꍇ��Rigidbody������
		auto PtrRegid = AddComponent<Rigidbody>();
		//Sphere�̏Փ˔��������
		auto CollPtr = AddComponent<CollisionSphere>();
		CollPtr->SetFixed(true);
		//�e������
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

		//m_SpeedMax���ő�l�Ɗ��Ⴗ��B���ۏo�Ă��鐔�l��m_SpeedMin�Ƒ��������l�Ȃ̂ňႤ

		m_Speed = Util::RandZeroToOne() * m_SpeedMax + m_SpeedMin;
		m_Radian = 0;
		m_Radius = Util::RandZeroToOne() * m_RadiusMax + m_RadiusMin;

		//Enemy�Ƃ����O���[�v�ɑg�ݍ���
		auto Group = GetStage()->GetSharedObjectGroup(L"Enemy");
		Group->IntoGroup(GetThis<EnemyObject>());

		AddTag(L"Enemy");
	}
	//�X�V����
	void EnemyObject::OnUpdate() {
		if (m_InfectedPercent < 100.0f) {
			Move();
		}
		else if (m_InfectedPercent >= 100.0f) {
			ToTargetPoint();
		}
		ColorChangeByInfection();
		Infect();
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
			PtrRigid->SetVelocity(MoveVec * 2.0f);
		}
		else {
			ToInfectMove();
		}
	}

	//��������100�����̈�ԋ߂��G�ɋ߂Â�����
	void EnemyObject::ToInfectMove() {
		float distance = 0.0f;
		Vec3 MoveVec = Vec3(0);
		auto& GroupVec = GetStage()->GetSharedObjectGroup(L"Enemy")->GetGroupVector();
		for (auto& v : GroupVec) {
			auto PtrEnemy = dynamic_pointer_cast<EnemyObject>(v.lock());
			if (PtrEnemy->GetInfectedPercent() < 100.0f) {
				float NowDis = (PtrEnemy->GetComponent<Transform>()->GetPosition() - GetComponent<Transform>()->GetPosition()).length();
				//�ŏ��̏�����
				if (distance == 0.0f) {
					distance = NowDis;
					MoveVec = PtrEnemy->GetComponent<Transform>()->GetPosition() - GetComponent<Transform>()->GetPosition();
					MoveVec.normalize();
				}
				//�ŒZ�������X�V����
				else if (distance > NowDis) {
					distance = NowDis;
					MoveVec = PtrEnemy->GetComponent<Transform>()->GetPosition() - GetComponent<Transform>()->GetPosition();
					MoveVec.normalize();
				}
			}
		}
		auto PtrRigid = GetComponent<Rigidbody>();
		PtrRigid->SetVelocity(MoveVec * 2.0f);
	}

	void EnemyObject::ColorChangeByInfection() {
		auto PtrDraw = GetComponent<BcPNTStaticDraw>();
		PtrDraw->SetDiffuse(Col4(m_InfectedPercent/100.0f, 0, m_InfectedPercent/100.0f, 1.0f));
	}

	void EnemyObject::Infect() {
		if (m_InfectedPercent > 0.0f) {
			//�G�t�F�N�g���o
			auto PtrSpark = GetStage()->GetSharedGameObject<InfectParticle>(L"InfectParticle", false);
			if (PtrSpark) {
				PtrSpark->InsertInfect(GetComponent<Transform>()->GetPosition(), m_InfectedPercent / 100.0f * m_InfectionLength);
			}

			//�ڋ߂�������̊��������グ��
			auto PtrTrans = GetComponent<Transform>();
			Vec3 MyPos = PtrTrans->GetPosition();
			auto& EnemyGroup = GetStage()->GetSharedObjectGroup(L"Enemy");
			for (auto& v : EnemyGroup->GetGroupVector()) {
				//���������
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

			//���g�̊��������グ��
			if (m_InfectedPercent < 100.0f) {
				m_InfectedPercent += 0.05f;
			}
			if (m_InfectedPercent > 100.0f) {
				m_InfectedPercent = 100.0f;
			}
		}
	}

}
//end basecross
