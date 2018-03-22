/*!
@file Character.h
@brief �L�����N�^�[�Ȃ�
*/

#pragma once
#include "stdafx.h"

namespace basecross{

	//--------------------------------------------------------------------------------------
	//	class FixedBox : public GameObject;
	//	�p�r: �Œ�̃{�b�N�X
	//--------------------------------------------------------------------------------------
	class FixedBox : public GameObject {
		Vec3 m_Scale;
		Vec3 m_Rotation;
		Vec3 m_Position;
	public:
		//�\�z�Ɣj��
		FixedBox(const shared_ptr<Stage>& StagePtr,
			const Vec3& Scale,
			const Vec3& Rotation,
			const Vec3& Position
		);
		virtual ~FixedBox();
		//������
		virtual void OnCreate() override;
	};

	//--------------------------------------------------------------------------------------
	//	class TargetObject : public GameObject;
	//	�p�r: ���������L�����N�^�[�̖ڕW�ƂȂ錚��
	//--------------------------------------------------------------------------------------
	class TargetObject : public GameObject {
		Vec3 m_Scale;
		Vec3 m_Rotation;
		Vec3 m_Position;

		//�ϋv�x
		float m_HP;
	public:
		//�\�z�Ɣj��
		TargetObject(const shared_ptr<Stage>& StagePtr,
			const Vec3& Scale,
			const Vec3& Rotation,
			const Vec3& Position
		);
		virtual ~TargetObject();
		//������
		virtual void OnCreate() override;
		//�X�V
		virtual void OnUpdate() override;
	};

	//--------------------------------------------------------------------------------------
	//	class EnemyObject : public GameObject;
	//	�p�r: ��ʂ̓G
	//--------------------------------------------------------------------------------------
	class EnemyObject : public GameObject {
		Vec3 m_StartPos;
        float m_Speed;
		float m_Radian;
		float m_Radius;

		float m_SpeedMax;
		float m_SpeedMin;
		float m_RadiusMax;
		float m_RadiusMin;

		//�v���C���[���������鋗��
		float m_SearchDistance;
		//�v���C���[�Ƃ̓G�΃X�s�[�h
		float m_OppositionSpeed;
		//������̍s���X�s�[�h
		float m_InfectedSpeed;
		//�����͈�
		float m_InfectionLength;
		//������,�ő�100.0f
		float m_InfectedPercent;
		//�K�w���X�e�[�g�}�V�[��
		unique_ptr<LayeredStateMachine<EnemyObject>>  m_StateMachine;
	public:
		//�\�z�Ɣj��
		EnemyObject(const shared_ptr<Stage>& StagePtr, const Vec3& StartPos);
		virtual ~EnemyObject();
		//������
		virtual void OnCreate() override;
		//����
		virtual void OnUpdate() override;

		void Move();

		//�������̃`�F�b�N
		void CheckInfection();

		//����Ɋ���������
		void Infect();

		//����������ړ�
		void ToInfectMove();

		//�v���C���[��T��
		void SearchPlayer();

		//�v���C���[�Ɍ������s��
		void ToPlayerMove();

		//�v���C���[�Ƀ_���[�W��^����
		void ToPlayerDamage();

		//�������ɂ���ĐF��ς���
		void ColorChangeByInfection();

		//�v���C���[�̎w�������ꏊ�Ɍ���������
		void ToTargetPoint();

		//�����͈͂�getter,setter
		float GetInfectionLength() {
			return m_InfectionLength;
		};
		void SetInfectionLength(float value) {
			m_InfectionLength = value;
		}
		//��������getter,setter
		float GetInfectedPercent() {
			return m_InfectedPercent;
		};
		void SetInfectedPercent(float value) {
			m_InfectedPercent = value;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�e�[�g�}�V���𓾂�
		@return	�X�e�[�g�}�V��
		*/
		//--------------------------------------------------------------------------------------
		unique_ptr<LayeredStateMachine<EnemyObject>>& GetStateMachine() {
			return m_StateMachine;
		}
	};

	//--------------------------------------------------------------------------------------
	/// �G�ʏ�X�e�[�g
	//--------------------------------------------------------------------------------------
	class EnemyDefaultState : public ObjState<EnemyObject>
	{
		EnemyDefaultState() {}
	public:
		//�X�e�[�g�̃C���X�^���X�擾
		DECLARE_SINGLETON_INSTANCE(EnemyDefaultState)
		virtual wstring GetStateName()const { return L"EnemyDefaultState"; }
		virtual void Enter(const shared_ptr<EnemyObject>& Obj)override;
		virtual void Execute(const shared_ptr<EnemyObject>& Obj)override;
		virtual void Exit(const shared_ptr<EnemyObject>& Obj)override;
	};

	//--------------------------------------------------------------------------------------
	///	�G�A�G�΃X�e�[�g
	//--------------------------------------------------------------------------------------
	class EnemyOppositionState : public ObjState<EnemyObject>
	{
		EnemyOppositionState() {}
	public:
		//�X�e�[�g�̃C���X�^���X�擾
		DECLARE_SINGLETON_INSTANCE(EnemyOppositionState)
		virtual wstring GetStateName()const { return L"EnemyOppositionState"; }
		virtual void Enter(const shared_ptr<EnemyObject>& Obj)override;
		virtual void Execute(const shared_ptr<EnemyObject>& Obj)override;
		virtual void Exit(const shared_ptr<EnemyObject>& Obj)override;
	};

	//--------------------------------------------------------------------------------------
	///	�G����������100%�ɂȂ薡���ɂȂ������̃X�e�[�g
	//--------------------------------------------------------------------------------------
	class EnemyInfectedState : public ObjState<EnemyObject>
	{
		EnemyInfectedState() {}
	public:
		//�X�e�[�g�̃C���X�^���X�擾
		DECLARE_SINGLETON_INSTANCE(EnemyInfectedState)
		virtual wstring GetStateName()const { return L"EnemyInfectedState"; }
		virtual void Enter(const shared_ptr<EnemyObject>& Obj)override;
		virtual void Execute(const shared_ptr<EnemyObject>& Obj)override;
		virtual void Exit(const shared_ptr<EnemyObject>& Obj)override;
	};

}
//end basecross
