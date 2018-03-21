/*!
@file Player.cpp
@brief �v���C���[�Ȃǎ���
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross{

	//--------------------------------------------------------------------------------------
	//	class AttackBall : public GameObject;
	//	�p�r: ���ł����{�[��
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
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


	//������
	void AttackBall::OnCreate() {
		auto PtrTransform = GetComponent<Transform>();

		PtrTransform->SetScale(0.2f, 0.2f, 0.2f);
		PtrTransform->SetRotation(0, 0, 0);
		PtrTransform->SetPosition(0, 0, 0);

		//Rigidbody������
		auto PtrRedid = AddComponent<Rigidbody>();

		//�Փ˔��������
		auto PtrCol = AddComponent<CollisionSphere>();

		//�e������
		auto ShadowPtr = AddComponent<Shadowmap>();
		ShadowPtr->SetMeshResource(L"DEFAULT_SPHERE");

		auto PtrDraw = AddComponent<BcPNTStaticDraw>();
		PtrDraw->SetMeshResource(L"DEFAULT_SPHERE");
		PtrDraw->SetDiffuse(Col4(1.0f, 0, 1.0f, 1.0f));
		PtrDraw->SetEmissive(Col4(1, 0, 1, 1));

		auto Group = GetStage()->GetSharedObjectGroup(L"AttackBall");
		Group->IntoGroup(GetThis<AttackBall>());

		//�ŏ��͖����ɂ��Ă���
		SetDrawActive(false);
		SetUpdateActive(false);

		AddTag(L"Bullet");
	}

	void AttackBall::OnUpdate() {
		
	}

	void AttackBall::OnCollision(vector<shared_ptr<GameObject>>& OtherVec) {
		//�G�t�F�N�g���o
		auto PtrSpark = GetStage()->GetSharedGameObject<MultiSpark>(L"MultiSpark", false);
		if (PtrSpark) {
			PtrSpark->InsertSpark(GetComponent<Transform>()->GetPosition());
		}
		//���M������
		auto PtrTransform = GetComponent<Transform>();
		PtrTransform->SetScale(0.1f, 0.1f, 0.1f);
		PtrTransform->SetRotation(0, 0, 0);
		PtrTransform->SetPosition(0, 0, 0);
		SetDrawActive(false);
		SetUpdateActive(false);
		//�Փ˔���
		for (auto v : OtherVec) {
			if (v->FindTag(L"Enemy")) {
				auto PtrEnemy = dynamic_pointer_cast<EnemyObject>(v);
				float NowPercent = PtrEnemy->GetInfectedPercent();
				float NewPercent = NowPercent + 20.0f;
				if (NewPercent > 100.0f) {
					NewPercent = 100.0f;
				}
				PtrEnemy->SetInfectedPercent(NewPercent);
			}
		}
	}

	//--------------------------------------------------------------------------------------
	//	class Player : public GameObject;
	//	�p�r: �v���C���[
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
	Player::Player(const shared_ptr<Stage>& StagePtr) :
		GameObject(StagePtr)
	{
		m_HP = 100.0f;
	}

	//������
	void Player::OnCreate() {

		//�����ʒu�Ȃǂ̐ݒ�
		auto Ptr = AddComponent<Transform>();
		Ptr->SetScale(0.5f, 0.5f, 0.5f);
		Ptr->SetRotation(0.0f, 0.0f, 0.0f);
		Ptr->SetPosition(0, 0.125f, 0);

		//Rigidbody������
		auto PtrRedid = AddComponent<Rigidbody>();
		//�Փ˔��������
		auto PtrCol = AddComponent<CollisionSphere>();
		PtrCol->SetIsHitAction(IsHitAction::Auto);
		//�����������
		auto PtrString = AddComponent<StringSprite>();
		PtrString->SetText(L"");
		PtrString->SetTextRect(Rect2D<float>(16.0f, 16.0f, 640.0f, 480.0f));

		//�e������i�V���h�E�}�b�v��`�悷��j
		auto ShadowPtr = AddComponent<Shadowmap>();
		//�e�̌`�i���b�V���j��ݒ�
		ShadowPtr->SetMeshResource(L"DEFAULT_SPHERE");

		//�`��R���|�[�l���g�̐ݒ�
		auto PtrDraw = AddComponent<BcPNTStaticDraw>();
		//�`�悷�郁�b�V����ݒ�
		PtrDraw->SetMeshResource(L"DEFAULT_SPHERE");
		//�`�悷��e�N�X�`����ݒ�
		//PtrDraw->SetTextureResource(L"TRACE_TX");
		PtrDraw->SetDiffuse(Col4(0.0f, 0.2f, 1.0f, 1.0f));
		//PtrDraw->SetEmissive(Col4(0, 1, 0, 1));
		//��������
		SetAlphaActive(true);

		//�J�����𓾂�
		auto PtrCamera = dynamic_pointer_cast<MyCamera>(OnGetDrawCamera());
		if (PtrCamera) {
			//MyCamera�Œ��ڂ���I�u�W�F�N�g�i�v���C���[�j�̐ݒ�
			PtrCamera->SetTargetObject(GetThis<GameObject>());
			PtrCamera->SetTargetToAt(Vec3(0, 0.75f, 0));
		}

		//�ŏ���A�{�^���̓W�����v
		m_PlayerAction = PlayerAction::Attack;
		//�X�e�[�g�}�V���̍\�z
		m_StateMachine.reset(new LayeredStateMachine<Player>(GetThis<Player>()));
		//�ŏ��̃X�e�[�g��PlayerDefault�Ƀ��Z�b�g
		m_StateMachine->Reset(PlayerDefaultState::Instance());
	}

	//�X�V
	void Player::OnUpdate() {
		//�R���g���[���`�F�b�N���ē��͂�����΃R�}���h�Ăяo��
		m_InputHandler.PushHandle(GetThis<Player>());
		//�X�e�[�g�}�V���X�V
		m_StateMachine->Update();
	}

	//��X�V
	void Player::OnUpdate2() {
		//������̕\��
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
			//���݂��W�����v�X�e�[�g���ړ��{�b�N�X�X�e�[�g�Ȃ�PlayerDefault�Ƀ��Z�b�g
			GetStateMachine()->Reset(PlayerDefaultState::Instance());
		}
	}

	//R�{�^��
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

	//B�{�^��
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

	//������̕\��
	void Player::DrawStrings() {

		//������\��
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
		//�����������
		auto PtrString = GetComponent<StringSprite>();
		PtrString->SetFontColor(Col4(0, 0, 0, 1));
		PtrString->SetText(str);
	}


	//--------------------------------------------------------------------------------------
	///	�ʏ�X�e�[�g
	//--------------------------------------------------------------------------------------

	IMPLEMENT_SINGLETON_INSTANCE(PlayerDefaultState)

		void PlayerDefaultState::Enter(const shared_ptr<Player>& Obj) {
		//�������Ȃ�
	}

	void PlayerDefaultState::Execute(const shared_ptr<Player>& Obj) {
		auto PtrBehavior = Obj->GetBehavior<PlayerBehavior>();
		PtrBehavior->MovePlayer();
		auto PtrGrav = Obj->GetBehavior<Gravity>();
		PtrGrav->Execute();
	}

	void PlayerDefaultState::Exit(const shared_ptr<Player>& Obj) {
		//�������Ȃ�
	}

	//--------------------------------------------------------------------------------------
	///	MoveBox�ɏ���Ă���X�e�[�g
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
	///	�W�����v�X�e�[�g
	//--------------------------------------------------------------------------------------
	IMPLEMENT_SINGLETON_INSTANCE(PlayerJumpState)
	void PlayerJumpState::Enter(const shared_ptr<Player>& Obj) {
		auto PtrGrav = Obj->GetBehavior<Gravity>();
		PtrGrav->StartJump(Vec3(0, 4.0f, 0));
	}

	void PlayerJumpState::Execute(const shared_ptr<Player>& Obj) {
		//�W�����v���������ύX�\
		auto PtrBehavior = Obj->GetBehavior<PlayerBehavior>();
		PtrBehavior->MovePlayer();
		auto PtrGrav = Obj->GetBehavior<Gravity>();
		PtrGrav->Execute();
	}

	void PlayerJumpState::Exit(const shared_ptr<Player>& Obj) {
		//�������Ȃ�
	}

	//--------------------------------------------------------------------------------------
	///	�A�^�b�N�X�e�[�g
	//--------------------------------------------------------------------------------------
	IMPLEMENT_SINGLETON_INSTANCE(PlayerAttackState)

		void PlayerAttackState::Enter(const shared_ptr<Player>& Obj) {
		auto PtrBehavior = Obj->GetBehavior<PlayerBehavior>();
		PtrBehavior->FireAttackBall();
	}

	void PlayerAttackState::Execute(const shared_ptr<Player>& Obj) {
		//�����ɃX�e�[�g��߂�
		Obj->GetStateMachine()->Pop();
	}

	void PlayerAttackState::Exit(const shared_ptr<Player>& Obj) {
		//�������Ȃ�
	}

	//--------------------------------------------------------------------------------------
	//	class FollowerTargetPoint : public GameObject;
	//	�p�r: �������������|�C���g
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
	FollowerTargetPoint::FollowerTargetPoint(const shared_ptr<Stage>& StagePtr) :
		GameObject(StagePtr)
	{}

	//������
	void FollowerTargetPoint::OnCreate() {

		//�����ʒu�Ȃǂ̐ݒ�
		auto Ptr = AddComponent<Transform>();
		Ptr->SetScale(0.5f, 0.5f, 0.5f);
		Ptr->SetRotation(0.0f, 0.0f, 0.0f);
		Ptr->SetPosition(0.0f, 0, 1.0f);

		//�`��R���|�[�l���g�̐ݒ�
		auto PtrDraw = AddComponent<BcPNTStaticDraw>();
		//�`�悷�郁�b�V����ݒ�
		PtrDraw->SetMeshResource(L"DEFAULT_SPHERE");
		PtrDraw->SetDiffuse(Col4(1, 1, 0, 1));
		PtrDraw->SetEmissive(Col4(1, 1, 0, 1));
		//�`�悷��e�N�X�`����ݒ�
		//PtrDraw->SetTextureResource(L"TRACE_TX");
		//��������
		SetAlphaActive(true);

		m_TotalTime = 0.0f;

		m_isSetTarget = false;

		//auto PtrPlayer = GetStage()->GetSharedGameObject<Player>(L"Player");
		//GetComponent<Transform>()->SetParent(PtrPlayer);
	}

	//�X�V
	void FollowerTargetPoint::OnUpdate() {
		//�O��̃^�[������̌o�ߎ��Ԃ����߂�
		float ElapsedTime = App::GetApp()->GetElapsedTime();
		m_TotalTime += ElapsedTime * 10.0f;
		if (m_TotalTime >= XM_2PI) {
			m_TotalTime = 0;
		}
		auto Pos = GetComponent<Transform>()->GetPosition();
		Pos.y = (sin(m_TotalTime) + 1.0f) * 0.2f;
		GetComponent<Transform>()->SetPosition(Pos);

		auto CntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		if (CntlVec[0].bConnected) {
			if (CntlVec[0].wPressedButtons & XINPUT_GAMEPAD_A) {
				m_isSetTarget = false;
			}
			if (m_isSetTarget == false) {
				if (CntlVec[0].bLeftTrigger > 0.0f) {
					SetDrawActive(true);
					//�J�����𓾂�
					auto PtrCamera = dynamic_pointer_cast<MyCamera>(OnGetDrawCamera());
					if (PtrCamera) {
						Vec3 CameraPos = PtrCamera->GetEye();

						if (CntlVec[0].fThumbRY > 0.0f) {
							Vec3 MoveVec = Pos - CameraPos;
							MoveVec.normalize();
							GetComponent<Transform>()->SetPosition(Pos + MoveVec * 0.3f);
						}
						if (CntlVec[0].fThumbRY < 0.0f) {
							Vec3 MoveVec = CameraPos - Pos;
							MoveVec.normalize();
							GetComponent<Transform>()->SetPosition(Pos + MoveVec * 0.3f);
						}
					}
					if (CntlVec[0].bRightTrigger > 0.0f) {
						m_isSetTarget = true;
					}
				}
				else
				{
					SetDrawActive(false);
					//�J�����𓾂�
					auto PtrCamera = dynamic_pointer_cast<MyCamera>(OnGetDrawCamera());
					if (PtrCamera) {
						Vec3 CameraPos = PtrCamera->GetEye();
						auto PtrPlayer = GetStage()->GetSharedGameObject<Player>(L"Player");
						Vec3 PlayerPos = PtrPlayer->GetComponent<Transform>()->GetPosition();
						Vec3 MoveVec = PlayerPos - CameraPos;
						MoveVec.y = 0.0f;
						MoveVec.normalize();
						GetComponent<Transform>()->SetPosition(PlayerPos + MoveVec * 2.0f);
					}
					
				}
			}
		}
	}

	//��X�V
	void FollowerTargetPoint::OnUpdate2() {
	}

	//--------------------------------------------------------------------------------------
	//	class CameraTargetPoint : public GameObject;
	//	�p�r: �J�����̒��ړ_
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
	CameraTargetPoint::CameraTargetPoint(const shared_ptr<Stage>& StagePtr) :
		GameObject(StagePtr)
	{}

	//������
	void CameraTargetPoint::OnCreate() {

		//�����ʒu�Ȃǂ̐ݒ�
		auto Ptr = AddComponent<Transform>();
		Ptr->SetScale(0.1f, 0.1f, 0.1f);
		Ptr->SetRotation(0.0f, 0.0f, 0.0f);
		Ptr->SetPosition(0.0f, 0, 1.0f);

		//�`��R���|�[�l���g�̐ݒ�
		auto PtrDraw = AddComponent<BcPNTStaticDraw>();
		//�`�悷�郁�b�V����ݒ�
		PtrDraw->SetMeshResource(L"DEFAULT_SPHERE");
		PtrDraw->SetDiffuse(Col4(1, 0, 0, 1));
		PtrDraw->SetEmissive(Col4(1, 0, 0, 1));
		//�`�悷��e�N�X�`����ݒ�
		//PtrDraw->SetTextureResource(L"TRACE_TX");
		//��������
		SetAlphaActive(true);

		auto PtrPlayer = GetStage()->GetSharedGameObject<Player>(L"Player");
		GetComponent<Transform>()->SetParent(PtrPlayer);
	}

	//�X�V
	void CameraTargetPoint::OnUpdate() {
	}

	//��X�V
	void CameraTargetPoint::OnUpdate2() {
	}

	//--------------------------------------------------------------------------------------
	//	HP�Q�[�W�X�v���C�g(�΂̕���)
	//--------------------------------------------------------------------------------------
	HPGaugeSprite::HPGaugeSprite(const shared_ptr<Stage>& StagePtr, bool Trace,
		const Vec3 & StartPos, const wstring& Media) :
		GameObject(StagePtr),
		m_Trace(Trace),
		m_StartPos(StartPos),
		m_Media(Media),
		m_TotalTime(0)
	{
		m_GaugeSize = 300.0f;
		m_DefaultGaugeSize = m_GaugeSize;
	}
	//�j��
	HPGaugeSprite::~HPGaugeSprite() {}

	void HPGaugeSprite::OnCreate() {
		float HalfSize = 0.5f;
		//���_�̔z��
		m_BackupVertices = {
			{ VertexPositionTexture(Vec3(-0,HalfSize,0),Vec2(0,0)) },
			{ VertexPositionTexture(Vec3(HalfSize*2.0f,HalfSize,0),Vec2(1,0)) },
			{ VertexPositionTexture(Vec3(-0,-HalfSize,0),Vec2(0,1)) },
			{ VertexPositionTexture(Vec3(HalfSize*2.0f,-HalfSize,0),Vec2(1,1)) },
		};

		//�C���f�b�N�X�z��
		vector<uint16_t> indices = { 0, 1, 2, 1 ,3 ,2 };
		SetAlphaActive(m_Trace);
		auto PtrTransform = GetComponent<Transform>();
		PtrTransform->SetScale(m_DefaultGaugeSize, 26.0f* 1.2f, 1.0f);
		PtrTransform->SetRotation(0, 0, 0);
		PtrTransform->SetPivot(0.0f, 0.0f, 0.0f);
		PtrTransform->SetPosition(m_StartPos);

		//���_�ƃC���f�b�N�X���w�肵�ăX�v���C�g�쐬
		auto Ptr = AddComponent<PTSpriteDraw>(m_BackupVertices, indices);
		Ptr->SetSamplerState(SamplerState::LinearWrap);
		Ptr->SetTextureResource(m_Media);
	}

	void HPGaugeSprite::OnUpdate() {
		GetPlayerHP();
		auto PtrTransform = GetComponent<Transform>();
		PtrTransform->SetScale(m_GaugeSize, 26.0f* 1.2f, 1.0f);
	}

	void HPGaugeSprite::GetPlayerHP() {
		auto PtrPlayer = GetStage()->GetSharedGameObject<Player>(L"Player");
		float NowHP = PtrPlayer->GetHP();
		m_GaugeSize = m_DefaultGaugeSize * NowHP / 100.0f;
	}
}
//end basecross

