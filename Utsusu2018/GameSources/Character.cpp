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
		PtrDraw->SetTextureResource(L"SKY_TX");
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
		m_InfectionLength = 0.0f;
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

		m_Speed = 0.5f;
		m_Radian = 0;
		m_Radius = 5.0f;

		//Enemy�Ƃ����O���[�v�ɑg�ݍ���
		auto Group = GetStage()->GetSharedObjectGroup(L"Enemy");
		Group->IntoGroup(GetThis<EnemyObject>());

		AddTag(L"Enemy");
	}
	//�X�V����
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
			//�G�t�F�N�g���o
			auto PtrSpark = GetStage()->GetSharedGameObject<InfectParticle>(L"InfectParticle", false);
			if (PtrSpark) {
				PtrSpark->InsertInfect(GetComponent<Transform>()->GetPosition(), m_InfectedPercent / 100.0f);
			}

			//�ڋ߂�������̊��������グ��
			auto& EnemyGroup = GetStage()->GetSharedObjectGroup(L"Enemy");
			for (auto& v : EnemyGroup->GetGroupVector()) {
				//���������
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
			//���g�̊��������グ��
			if (m_InfectedPercent < 100.0f) {
				m_InfectedPercent += 0.05f;
			}
		}
	}

}
//end basecross
