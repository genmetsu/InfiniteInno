/*!
@file Player.h
@brief プレイヤーなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{

	//--------------------------------------------------------------------------------------
	///	飛んでいくボール
	//--------------------------------------------------------------------------------------
	class AttackBall : public GameObject {

	public:
		//構築と破棄
		AttackBall(const shared_ptr<Stage>& StagePtr);
		virtual ~AttackBall();
		void Weakup(const Vec3& Position, const Vec3& Velocity);
		//初期化
		virtual void OnCreate() override;
		//操作
		virtual void OnUpdate() override;
		//衝突
		virtual void OnCollision(vector<shared_ptr<GameObject>>& OtherVec) override;
	};


	//Aボタンで動作するアクション
	enum class PlayerAction {
		Jump,
		Attack
	};

	//--------------------------------------------------------------------------------------
	///	プレイヤー
	//--------------------------------------------------------------------------------------
	class Player : public GameObject {
		//文字列の表示
		void DrawStrings();
		//入力ハンドラー
		InputHandler<Player> m_InputHandler;

		float m_HP;

		//プレイヤーのAボタンによる行動
		PlayerAction m_PlayerAction;
		//階層化ステートマシーン
		unique_ptr<LayeredStateMachine<Player>>  m_StateMachine;
	public:
		//構築と破棄
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		@param[in]	StagePtr	ステージ
		*/
		//--------------------------------------------------------------------------------------
		Player(const shared_ptr<Stage>& StagePtr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~Player() {}
		//アクセサ
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ステートマシンを得る
		@return	ステートマシン
		*/
		//--------------------------------------------------------------------------------------
		unique_ptr<LayeredStateMachine<Player>>& GetStateMachine() {
			return m_StateMachine;
		}
		//初期化
		virtual void OnCreate() override;
		//更新
		virtual void OnUpdate() override;
		//後更新
		virtual void OnUpdate2() override;
		//衝突時
		virtual void OnCollision(vector<shared_ptr<GameObject>>& OtherVec) override;

		float GetHP() {
			return m_HP;
		}
		void SetHP(float value) {
			m_HP = value;
		}

		//Lボタン
		void OnPushRightSoulder();
		//Bボタン
		void OnPushB();
	};

	//--------------------------------------------------------------------------------------
	///	味方が向かうポイント
	//--------------------------------------------------------------------------------------
	class FollowerTargetPoint : public GameObject {
		float m_TotalTime;
		bool m_isSetTarget;
	public:
		//構築と破棄
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		@param[in]	StagePtr	ステージ
		*/
		//--------------------------------------------------------------------------------------
		FollowerTargetPoint(const shared_ptr<Stage>& StagePtr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~FollowerTargetPoint() {}
		//初期化
		virtual void OnCreate() override;
		//更新
		virtual void OnUpdate() override;
		//後更新
		virtual void OnUpdate2() override;
	};

	//--------------------------------------------------------------------------------------
	///	カメラのターゲットポイント
	//--------------------------------------------------------------------------------------
	class CameraTargetPoint : public GameObject {
	public:
		//構築と破棄
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		@param[in]	StagePtr	ステージ
		*/
		//--------------------------------------------------------------------------------------
		CameraTargetPoint(const shared_ptr<Stage>& StagePtr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~CameraTargetPoint() {}
		//初期化
		virtual void OnCreate() override;
		//更新
		virtual void OnUpdate() override;
		//後更新
		virtual void OnUpdate2() override;
	};

	//--------------------------------------------------------------------------------------
	///	通常ステート
	//--------------------------------------------------------------------------------------
	class PlayerDefaultState : public ObjState<Player>
	{
		PlayerDefaultState() {}
	public:
		//ステートのインスタンス取得
		DECLARE_SINGLETON_INSTANCE(PlayerDefaultState)
		virtual wstring GetStateName()const { return L"PlayerDefaultState"; }
		virtual void Enter(const shared_ptr<Player>& Obj)override;
		virtual void Execute(const shared_ptr<Player>& Obj)override;
		virtual void Exit(const shared_ptr<Player>& Obj)override;
	};

	//--------------------------------------------------------------------------------------
	///	MoveBoxに乗っているステート
	//--------------------------------------------------------------------------------------
	class PlayerOnMoveboxState : public ObjState<Player>
	{
		PlayerOnMoveboxState() {}
	public:
		//ステートのインスタンス取得
		DECLARE_SINGLETON_INSTANCE(PlayerOnMoveboxState)
		virtual wstring GetStateName()const { return L"PlayerOnMoveboxState"; }
		virtual void Enter(const shared_ptr<Player>& Obj)override;
		virtual void Execute(const shared_ptr<Player>& Obj)override;
		virtual void Sleep(const shared_ptr<Player>& Obj)override;
		virtual void Exit(const shared_ptr<Player>& Obj)override;
	};

	//--------------------------------------------------------------------------------------
	///	ジャンプステート
	//--------------------------------------------------------------------------------------
	class PlayerJumpState : public ObjState<Player>
	{
		PlayerJumpState() {}
	public:
		//ステートのインスタンス取得
		DECLARE_SINGLETON_INSTANCE(PlayerJumpState)
		virtual wstring GetStateName()const { return L"PlayerJumpState"; }
		virtual void Enter(const shared_ptr<Player>& Obj)override;
		virtual void Execute(const shared_ptr<Player>& Obj)override;
		virtual void Exit(const shared_ptr<Player>& Obj)override;
	};


	//--------------------------------------------------------------------------------------
	///	アタックステート
	//--------------------------------------------------------------------------------------
	class PlayerAttackState : public ObjState<Player>
	{
		PlayerAttackState() {}
	public:
		//ステートのインスタンス取得
		DECLARE_SINGLETON_INSTANCE(PlayerAttackState)
		virtual wstring GetStateName()const { return L"PlayerAttackState"; }
		virtual void Enter(const shared_ptr<Player>& Obj)override;
		virtual void Execute(const shared_ptr<Player>& Obj)override;
		virtual void Exit(const shared_ptr<Player>& Obj)override;
	};

	//--------------------------------------------------------------------------------------
	///	プレイヤーのHPゲージスプライト(緑の部分)
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

