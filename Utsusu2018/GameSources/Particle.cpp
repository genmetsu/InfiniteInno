#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//class MultiSpark : public MultiParticle;
	//用途: 複数のスパーククラス
	//--------------------------------------------------------------------------------------
	//構築と破棄
	MultiSpark::MultiSpark(shared_ptr<Stage>& StagePtr) :
		MultiParticle(StagePtr)
	{}
	MultiSpark::~MultiSpark() {}

	//初期化
	void MultiSpark::OnCreate() {
	}

	void MultiSpark::InsertSpark(const Vec3& Pos) {
		auto ParticlePtr = InsertParticle(30);
		ParticlePtr->SetEmitterPos(Pos);
		ParticlePtr->SetTextureResource(L"SPARK_TX");
		ParticlePtr->SetMaxTime(0.3f);
		vector<ParticleSprite>& pSpriteVec = ParticlePtr->GetParticleSpriteVec();
		for (auto& rParticleSprite : ParticlePtr->GetParticleSpriteVec()) {
			rParticleSprite.m_LocalPos.x = Util::RandZeroToOne() * 0.1f - 0.05f;
			rParticleSprite.m_LocalPos.y = Util::RandZeroToOne() * 0.1f;
			rParticleSprite.m_LocalPos.z = Util::RandZeroToOne() * 0.1f - 0.05f;
			rParticleSprite.m_LocalScale = Vec2(0.2f, 0.2f);
			//各パーティクルの移動速度を指定
			rParticleSprite.m_Velocity = Vec3(
				rParticleSprite.m_LocalPos.x * 40.0f,
				rParticleSprite.m_LocalPos.y * 40.0f,
				rParticleSprite.m_LocalPos.z * 40.0f
			);
			//色の指定
			rParticleSprite.m_Color = Col4(1.0f, 0.0f, 1.0f, 1.0f);
		}
	}

	void MultiSpark::OnUpdate() {
		MultiParticle::OnUpdate();
		for (auto ParticlePtr : GetParticleVec()) {
			for (auto& rParticleSprite : ParticlePtr->GetParticleSpriteVec()) {
				if (rParticleSprite.m_Active) {
					rParticleSprite.m_Color.w -= 0.05f;
				}
			}
		}
	}

	//--------------------------------------------------------------------------------------
	//class InfectParticle : public MultiParticle;
	//用途: 感染している際のパーティクル
	//--------------------------------------------------------------------------------------
	//構築と破棄
	InfectParticle::InfectParticle(shared_ptr<Stage>& StagePtr) :
		MultiParticle(StagePtr)
	{}
	InfectParticle::~InfectParticle() {}

	//初期化
	void InfectParticle::OnCreate() {
	}

	void InfectParticle::InsertInfect(const Vec3& Pos, const float Length) {
		auto ParticlePtr = InsertParticle((int)(Length + 1) * 4);
		ParticlePtr->SetEmitterPos(Pos);
		ParticlePtr->SetTextureResource(L"SPARK_TX");
		ParticlePtr->SetMaxTime(0.5f);
		vector<ParticleSprite>& pSpriteVec = ParticlePtr->GetParticleSpriteVec();
		for (auto& rParticleSprite : ParticlePtr->GetParticleSpriteVec()) {
			rParticleSprite.m_LocalPos.x = Util::RandZeroToOne() * Length - (Length / 2.0f);
			rParticleSprite.m_LocalPos.y = 0.0f;
			rParticleSprite.m_LocalPos.z = Util::RandZeroToOne() * Length - (Length / 2.0f);
			rParticleSprite.m_LocalScale = Vec2(0.6f, 0.6f);
			//各パーティクルの移動速度を指定
			rParticleSprite.m_Velocity = Vec3(
				rParticleSprite.m_LocalPos.x * 2.0f,
				rParticleSprite.m_LocalPos.y * 10.0f,
				rParticleSprite.m_LocalPos.z * 2.0f
			);
			//色の指定
			rParticleSprite.m_Color = Col4(1.0f, 0.0f, 1.0f, 1.0f);
		}
	}

	void InfectParticle::OnUpdate() {
		MultiParticle::OnUpdate();
		for (auto ParticlePtr : GetParticleVec()) {
			for (auto& rParticleSprite : ParticlePtr->GetParticleSpriteVec()) {
				if (rParticleSprite.m_Active) {
					rParticleSprite.m_Color.w -= 0.01f;
				}
			}
		}
	}

}