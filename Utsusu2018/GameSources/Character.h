/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{

	//--------------------------------------------------------------------------------------
	//	class FixedBox : public GameObject;
	//	用途: 固定のボックス
	//--------------------------------------------------------------------------------------
	class FixedBox : public GameObject {
		Vec3 m_Scale;
		Vec3 m_Rotation;
		Vec3 m_Position;
	public:
		//構築と破棄
		FixedBox(const shared_ptr<Stage>& StagePtr,
			const Vec3& Scale,
			const Vec3& Rotation,
			const Vec3& Position
		);
		virtual ~FixedBox();
		//初期化
		virtual void OnCreate() override;
	};

	//--------------------------------------------------------------------------------------
	//	class TargetObject : public GameObject;
	//	用途: 感染したキャラクターの目標となる建物
	//--------------------------------------------------------------------------------------
	class TargetObject : public GameObject {
		Vec3 m_Scale;
		Vec3 m_Rotation;
		Vec3 m_Position;

		//耐久度
		float m_HP;
	public:
		//構築と破棄
		TargetObject(const shared_ptr<Stage>& StagePtr,
			const Vec3& Scale,
			const Vec3& Rotation,
			const Vec3& Position
		);
		virtual ~TargetObject();
		//初期化
		virtual void OnCreate() override;
		//更新
		virtual void OnUpdate() override;
	};

	//--------------------------------------------------------------------------------------
	//	class EnemyObject : public GameObject;
	//	用途: 一般の敵
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

		//感染範囲
		float m_InfectionLength;
		//感染率,最大100.0f
		float m_InfectedPercent;

	public:
		//構築と破棄
		EnemyObject(const shared_ptr<Stage>& StagePtr, const Vec3& StartPos);
		virtual ~EnemyObject();
		//初期化
		virtual void OnCreate() override;
		//操作
		virtual void OnUpdate() override;

		void Move();

		//周りに感染させる
		void Infect();

		//感染させる移動
		void ToInfectMove();

		//感染率によって色を変える
		void ColorChangeByInfection();

		//プレイヤーの指示した場所に向かう処理
		void ToTargetPoint();

		//感染範囲のgetter,setter
		float GetInfectionLength() {
			return m_InfectionLength;
		};
		void SetInfectionLength(float value) {
			m_InfectionLength = value;
		}
		//感染率のgetter,setter
		float GetInfectedPercent() {
			return m_InfectedPercent;
		};
		void SetInfectedPercent(float value) {
			m_InfectedPercent = value;
		}
	};

}
//end basecross
