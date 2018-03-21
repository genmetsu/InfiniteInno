/*!
@file Player.h
@brief �v���C���[�Ȃ�
*/

#pragma once
#include "stdafx.h"

namespace basecross{

	//--------------------------------------------------------------------------------------
	///	���ł����{�[��
	//--------------------------------------------------------------------------------------
	class AttackBall : public GameObject {

	public:
		//�\�z�Ɣj��
		AttackBall(const shared_ptr<Stage>& StagePtr);
		virtual ~AttackBall();
		void Weakup(const Vec3& Position, const Vec3& Velocity);
		//������
		virtual void OnCreate() override;
		//����
		virtual void OnUpdate() override;
		//�Փ�
		virtual void OnCollision(vector<shared_ptr<GameObject>>& OtherVec) override;
	};


	//A�{�^���œ��삷��A�N�V����
	enum class PlayerAction {
		Jump,
		Attack
	};

	//--------------------------------------------------------------------------------------
	///	�v���C���[
	//--------------------------------------------------------------------------------------
	class Player : public GameObject {
		//������̕\��
		void DrawStrings();
		//���̓n���h���[
		InputHandler<Player> m_InputHandler;

		float m_HP;

		//�v���C���[��A�{�^���ɂ��s��
		PlayerAction m_PlayerAction;
		//�K�w���X�e�[�g�}�V�[��
		unique_ptr<LayeredStateMachine<Player>>  m_StateMachine;
	public:
		//�\�z�Ɣj��
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�g���N�^
		@param[in]	StagePtr	�X�e�[�W
		*/
		//--------------------------------------------------------------------------------------
		Player(const shared_ptr<Stage>& StagePtr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�X�g���N�^
		*/
		//--------------------------------------------------------------------------------------
		virtual ~Player() {}
		//�A�N�Z�T
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�e�[�g�}�V���𓾂�
		@return	�X�e�[�g�}�V��
		*/
		//--------------------------------------------------------------------------------------
		unique_ptr<LayeredStateMachine<Player>>& GetStateMachine() {
			return m_StateMachine;
		}
		//������
		virtual void OnCreate() override;
		//�X�V
		virtual void OnUpdate() override;
		//��X�V
		virtual void OnUpdate2() override;
		//�Փˎ�
		virtual void OnCollision(vector<shared_ptr<GameObject>>& OtherVec) override;

		float GetHP() {
			return m_HP;
		}
		void SetHP(float value) {
			m_HP = value;
		}

		//L�{�^��
		void OnPushRightSoulder();
		//B�{�^��
		void OnPushB();
	};

	//--------------------------------------------------------------------------------------
	///	�������������|�C���g
	//--------------------------------------------------------------------------------------
	class FollowerTargetPoint : public GameObject {
		float m_TotalTime;
		bool m_isSetTarget;
	public:
		//�\�z�Ɣj��
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�g���N�^
		@param[in]	StagePtr	�X�e�[�W
		*/
		//--------------------------------------------------------------------------------------
		FollowerTargetPoint(const shared_ptr<Stage>& StagePtr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�X�g���N�^
		*/
		//--------------------------------------------------------------------------------------
		virtual ~FollowerTargetPoint() {}
		//������
		virtual void OnCreate() override;
		//�X�V
		virtual void OnUpdate() override;
		//��X�V
		virtual void OnUpdate2() override;
	};

	//--------------------------------------------------------------------------------------
	///	�J�����̃^�[�Q�b�g�|�C���g
	//--------------------------------------------------------------------------------------
	class CameraTargetPoint : public GameObject {
	public:
		//�\�z�Ɣj��
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�g���N�^
		@param[in]	StagePtr	�X�e�[�W
		*/
		//--------------------------------------------------------------------------------------
		CameraTargetPoint(const shared_ptr<Stage>& StagePtr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�X�g���N�^
		*/
		//--------------------------------------------------------------------------------------
		virtual ~CameraTargetPoint() {}
		//������
		virtual void OnCreate() override;
		//�X�V
		virtual void OnUpdate() override;
		//��X�V
		virtual void OnUpdate2() override;
	};

	//--------------------------------------------------------------------------------------
	///	�ʏ�X�e�[�g
	//--------------------------------------------------------------------------------------
	class PlayerDefaultState : public ObjState<Player>
	{
		PlayerDefaultState() {}
	public:
		//�X�e�[�g�̃C���X�^���X�擾
		DECLARE_SINGLETON_INSTANCE(PlayerDefaultState)
		virtual wstring GetStateName()const { return L"PlayerDefaultState"; }
		virtual void Enter(const shared_ptr<Player>& Obj)override;
		virtual void Execute(const shared_ptr<Player>& Obj)override;
		virtual void Exit(const shared_ptr<Player>& Obj)override;
	};

	//--------------------------------------------------------------------------------------
	///	MoveBox�ɏ���Ă���X�e�[�g
	//--------------------------------------------------------------------------------------
	class PlayerOnMoveboxState : public ObjState<Player>
	{
		PlayerOnMoveboxState() {}
	public:
		//�X�e�[�g�̃C���X�^���X�擾
		DECLARE_SINGLETON_INSTANCE(PlayerOnMoveboxState)
		virtual wstring GetStateName()const { return L"PlayerOnMoveboxState"; }
		virtual void Enter(const shared_ptr<Player>& Obj)override;
		virtual void Execute(const shared_ptr<Player>& Obj)override;
		virtual void Sleep(const shared_ptr<Player>& Obj)override;
		virtual void Exit(const shared_ptr<Player>& Obj)override;
	};

	//--------------------------------------------------------------------------------------
	///	�W�����v�X�e�[�g
	//--------------------------------------------------------------------------------------
	class PlayerJumpState : public ObjState<Player>
	{
		PlayerJumpState() {}
	public:
		//�X�e�[�g�̃C���X�^���X�擾
		DECLARE_SINGLETON_INSTANCE(PlayerJumpState)
		virtual wstring GetStateName()const { return L"PlayerJumpState"; }
		virtual void Enter(const shared_ptr<Player>& Obj)override;
		virtual void Execute(const shared_ptr<Player>& Obj)override;
		virtual void Exit(const shared_ptr<Player>& Obj)override;
	};


	//--------------------------------------------------------------------------------------
	///	�A�^�b�N�X�e�[�g
	//--------------------------------------------------------------------------------------
	class PlayerAttackState : public ObjState<Player>
	{
		PlayerAttackState() {}
	public:
		//�X�e�[�g�̃C���X�^���X�擾
		DECLARE_SINGLETON_INSTANCE(PlayerAttackState)
		virtual wstring GetStateName()const { return L"PlayerAttackState"; }
		virtual void Enter(const shared_ptr<Player>& Obj)override;
		virtual void Execute(const shared_ptr<Player>& Obj)override;
		virtual void Exit(const shared_ptr<Player>& Obj)override;
	};

	//--------------------------------------------------------------------------------------
	///	�v���C���[��HP�Q�[�W�X�v���C�g(�΂̕���)
	//--------------------------------------------------------------------------------------
	class HPGaugeSprite : public GameObject {
		bool m_Trace;
		Vec2 m_StartScale;
		Vec3 m_StartPos;
		float m_TotalTime;
		wstring m_Media;

		float m_GaugeSize;
		float m_DefaultGaugeSize;

		vector<VertexPositionTexture> m_BackupVertices;
	public:
		HPGaugeSprite(const shared_ptr<Stage> & 
			StagePtr, bool Trace,
			const Vec3& StartPos, const wstring& Media);

		virtual ~HPGaugeSprite();
		virtual void OnCreate() override;
		virtual void OnUpdate() override;

		void GetPlayerHP();
	};

}
//end basecross

