#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//class MultiSpark : public MultiParticle;
	//�p�r: �����̃X�p�[�N�N���X
	//--------------------------------------------------------------------------------------
	class MultiSpark : public MultiParticle {
	public:
		//�\�z�Ɣj��
		MultiSpark(shared_ptr<Stage>& StagePtr);
		virtual ~MultiSpark();
		//������
		virtual void OnCreate() override;
		//�X�V
		virtual void OnUpdate() override;
		void InsertSpark(const Vec3& Pos);
	};

	//--------------------------------------------------------------------------------------
	//class InfectParticle : public MultiParticle;
	//�p�r: �������Ă���ۂ̃p�[�e�B�N��
	//--------------------------------------------------------------------------------------
	class InfectParticle : public MultiParticle {
	public:
		//�\�z�Ɣj��
		InfectParticle(shared_ptr<Stage>& StagePtr);
		virtual ~InfectParticle();
		//������
		virtual void OnCreate() override;
		//�X�V
		virtual void OnUpdate() override;
		void InsertInfect(const Vec3& Pos, const float Length);
	};

}