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

		//�����͈�
		float m_InfectionLength;
		//������,�ő�100.0f
		float m_InfectedPercent;

	public:
		//�\�z�Ɣj��
		EnemyObject(const shared_ptr<Stage>& StagePtr, const Vec3& StartPos);
		virtual ~EnemyObject();
		//������
		virtual void OnCreate() override;
		//����
		virtual void OnUpdate() override;

		void Move();

		//����Ɋ���������
		void Infect();

		//����������ړ�
		void ToInfectMove();

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
	};

}
//end basecross
