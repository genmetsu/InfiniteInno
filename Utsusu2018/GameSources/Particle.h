#pragma once
#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//class MultiSpark : public MultiParticle;
	//用途: 複数のスパーククラス
	//--------------------------------------------------------------------------------------
	class MultiSpark : public MultiParticle {
	public:
		//構築と破棄
		MultiSpark(shared_ptr<Stage>& StagePtr);
		virtual ~MultiSpark();
		//初期化
		virtual void OnCreate() override;
		//更新
		virtual void OnUpdate() override;
		void InsertSpark(const Vec3& Pos);
	};

	//--------------------------------------------------------------------------------------
	//class InfectParticle : public MultiParticle;
	//用途: 感染している際のパーティクル
	//--------------------------------------------------------------------------------------
	class InfectParticle : public MultiParticle {
	public:
		//構築と破棄
		InfectParticle(shared_ptr<Stage>& StagePtr);
		virtual ~InfectParticle();
		//初期化
		virtual void OnCreate() override;
		//更新
		virtual void OnUpdate() override;
		void InsertInfect(const Vec3& Pos, const float Length);
	};

}