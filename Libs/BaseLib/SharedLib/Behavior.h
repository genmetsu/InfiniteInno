/*!
@file Behavior.h
@brief 行動クラス
@copyright Copyright (c) 2017 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	class GameObject;
	//--------------------------------------------------------------------------------------
	///	行動クラスの親クラス
	//--------------------------------------------------------------------------------------
	class Behavior : public ObjectInterface {
	protected:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	プロテクトコンストラクタ
		@param[in]	GameObjectPtr	このコンポーネントを所持するゲームオブジェクト
		*/
		//--------------------------------------------------------------------------------------
		explicit Behavior(const shared_ptr<GameObject>& GameObjectPtr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	プロテクトデストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~Behavior();
	public:
		//アクセサ
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ゲームオブジェクトの取得
		@return	このコンポーネントを所持するゲームオブジェクト
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<GameObject> GetGameObject() const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	所属するステージの取得
		@return	ゲームオブジェクトが所属するステージ
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<Stage> GetStage() const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief 初期化(空関数、必要なら多重定義する)
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate() {}
	private:
		// pImplイディオム
		struct Impl;
		unique_ptr<Impl> pImpl;
	};

	//--------------------------------------------------------------------------------------
	///	行動ユーティリティクラス
	//--------------------------------------------------------------------------------------
	class UtilBehavior : public Behavior {
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		@param[in]	GameObjectPtr	このコンポーネントを所持するゲームオブジェクト
		*/
		//--------------------------------------------------------------------------------------
		UtilBehavior(const shared_ptr<GameObject>& GameObjectPtr) :
			Behavior(GameObjectPtr)
		{}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~UtilBehavior() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief 進行方向を向くようにする
		@param[in]	LerpFact	補間係数（0.0から1.0の間）
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void RotToHead(float LerpFact);
		//--------------------------------------------------------------------------------------
		/*!
		@brief 進行方向を向くようにする(速度指定方式)
		@param[in]	LerpFact	補間係数（0.0から1.0の間）
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void RotToHead(const bsm::Vec3& Velocity, float LerpFact);

	};

	//--------------------------------------------------------------------------------------
	///	何もしない行動クラス(速度は減速)
	//--------------------------------------------------------------------------------------
	class WaitBehavior : public Behavior {
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		@param[in]	GameObjectPtr	ゲームオブジェクト
		*/
		//--------------------------------------------------------------------------------------
		WaitBehavior(const shared_ptr<GameObject>& GameObjectPtr) :
			Behavior(GameObjectPtr)
		{}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~WaitBehavior() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	行動を実行する
		@param[in]	TargetPos	追いかける位置
		@return	追いかける位置との距離
		*/
		//--------------------------------------------------------------------------------------
		float Execute(const bsm::Vec3& TargetPos);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	行動を実行する
		@param[in]	TargetKey	追いかけるオブジェクトのキー（SharedObjec）
		@return	追いかけるオブジェクトとの距離
		*/
		//--------------------------------------------------------------------------------------
		float Execute(const wstring& TargetKey);
	};




	//--------------------------------------------------------------------------------------
	///	Gravity行動クラス
	//--------------------------------------------------------------------------------------
	class Gravity : public Behavior {
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	コンストラクタ
		@param[in]	GameObjectPtr	このコンポーネントを所持するゲームオブジェクト
		*/
		//--------------------------------------------------------------------------------------
		Gravity(const shared_ptr<GameObject>& GameObjectPtr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	デストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~Gravity();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	現在の重力を得る
		@return	現在の重力
		*/
		//--------------------------------------------------------------------------------------
		const bsm::Vec3& GetGravity() const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	現在の重力を設定する
		@param[in]	gravity	重力値
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetGravity(const bsm::Vec3& gravity);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	現在の重力を設定する
		@param[in]	x	重力値X
		@param[in]	y	重力値Y
		@param[in]	z	重力値Z
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetGravity(float x, float y, float z);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ジャンプのスタート
		@param[in]	StartVec	初速度
		@param[in]	EscapeSpan	ジャンプ開始地点への上部調整値
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void StartJump(const bsm::Vec3& StartVec, float EscapeSpan = 0.01f);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ジャンプのスタート
		@param[in]	x	初速度X
		@param[in]	y	初速度Y
		@param[in]	z	初速度Z
		@param[in]	EscapeSpan	ジャンプ開始地点への上部調整値
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void StartJump(float x, float y, float z, float EscapeSpan = 0.01f);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	指定した時間だけ重力をかける
		@param[in]	CalcTime	時間
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void UpdateFromTime(float CalcTime);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	行動を実行する
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void Execute();
	private:
		// pImplイディオム
		struct Impl;
		unique_ptr<Impl> pImpl;
	};

	//--------------------------------------------------------------------------------------
	///	行動クラスの親クラス(テンプレート版)
	//--------------------------------------------------------------------------------------
	template <typename T>
	class ObjBehavior : public ObjectInterface {
		weak_ptr<T> m_GameObject;
	protected:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	プロテクトコンストラクタ
		@param[in]	GameObjectPtr	このコンポーネントを所持するゲームオブジェクト
		*/
		//--------------------------------------------------------------------------------------
		explicit ObjBehavior(const shared_ptr<T>& GameObjectPtr):
			m_GameObject(GameObjectPtr)
		{}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	プロテクトデストラクタ
		*/
		//--------------------------------------------------------------------------------------
		virtual ~ObjBehavior() {}
	public:
		//アクセサ
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ゲームオブジェクトの取得
		@return	このコンポーネントを所持するゲームオブジェクト
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<T> GetGameObject() const {
			auto shptr = m_GameObject.lock();
			if (!shptr) {
				throw BaseException(
					L"GameObjectは有効ではありません",
					L"if (!shptr)",
					L"ObjBehavior::GetGameObject()"
				);
			}
			else {
				return shptr;
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	所属するステージの取得
		@return	ゲームオブジェクトが所属するステージ
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<Stage> GetStage() const {
			return GetGameObject()->GetStage();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief 初期化(空関数、必要なら多重定義する)
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate() {}
	};



}
//end basecross
