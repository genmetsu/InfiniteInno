/*!
@file ProjectBehavior.cpp
@brief プロジェク定義の行動クラス実体
*/

#include "stdafx.h"
#include "Project.h"


namespace basecross {

	//--------------------------------------------------------------------------------------
	//	struct MyCamera::Impl;
	//	用途: Implクラス
	//--------------------------------------------------------------------------------------
	struct MyCamera::Impl {
		weak_ptr<GameObject> m_TargetObject;	//目標となるオブジェクト
		float m_ToTargetLerp;	//目標を追いかける際の補間値
		bsm::Vec3 m_TargetToAt;	//目標から視点を調整する位置ベクトル

		float m_RadY;
		float m_RadXZ;
		//カメラの上下スピード
		float m_CameraUpDownSpeed;
		//カメラを下げる下限角度
		float m_CameraUnderRot;
		//腕の長さの設定
		float m_ArmLen;
		float m_MaxArm;
		float m_MinArm;
		//回転スピード
		float m_RotSpeed;
		//ズームスピード
		float m_ZoomSpeed;
		//左右スティック変更のモード
		bool m_LRBaseMode;
		//上下スティック変更のモード
		bool m_UDBaseMode;

		Impl() :
			m_ToTargetLerp(1.0f),
			m_TargetToAt(0, 0, 0),
			m_RadY(0.5f),
			m_RadXZ(0),
			m_CameraUpDownSpeed(2.0f),
			m_CameraUnderRot(0.0f),
			m_ArmLen(7.0f),
			m_MaxArm(7.0f),
			m_MinArm(2.0f),
			m_RotSpeed(0.5f),
			m_ZoomSpeed(0.5f),
			m_LRBaseMode(true),
			m_UDBaseMode(true)
		{}
		~Impl() {}
	};



	//--------------------------------------------------------------------------------------
	//	class MyCamera : public Camera ;
	//	用途: MyCamera（コンポーネントではない）
	//--------------------------------------------------------------------------------------
	//構築と破棄
	MyCamera::MyCamera() :
		Camera(),
		pImpl(new Impl())
	{}

	MyCamera::MyCamera(float ArmLen) :
		Camera(),
		pImpl(new Impl())
	{
		pImpl->m_ArmLen = ArmLen;
		auto Eye = GetEye();
		Eye.y = pImpl->m_ArmLen;
		SetEye(Eye);
	}

	MyCamera::~MyCamera() {}
	//アクセサ
	shared_ptr<GameObject> MyCamera::GetTargetObject() const {
		if (!pImpl->m_TargetObject.expired()) {
			return pImpl->m_TargetObject.lock();
		}
		return nullptr;
	}

	void MyCamera::SetTargetObject(const shared_ptr<GameObject>& Obj) {
		pImpl->m_TargetObject = Obj;
	}

	float MyCamera::GetToTargetLerp() const {
		return pImpl->m_ToTargetLerp;
	}
	void MyCamera::SetToTargetLerp(float f) {
		pImpl->m_ToTargetLerp = f;
	}

	float MyCamera::GetArmLengh() const {
		return pImpl->m_ArmLen;
	}

	void MyCamera::UpdateArmLengh() {
		auto Vec = GetEye() - GetAt();
		pImpl->m_ArmLen = bsm::length(Vec);
		if (pImpl->m_ArmLen >= pImpl->m_MaxArm) {
			//m_MaxArm以上離れないようにする
			pImpl->m_ArmLen = pImpl->m_MaxArm;
		}
		if (pImpl->m_ArmLen <= pImpl->m_MinArm) {
			//m_MinArm以下近づかないようにする
			pImpl->m_ArmLen = pImpl->m_MinArm;
		}
	}

	float MyCamera::GetMaxArm() const {
		return pImpl->m_MaxArm;

	}
	void MyCamera::SetMaxArm(float f) {
		pImpl->m_MaxArm = f;
	}
	float MyCamera::GetMinArm() const {
		return pImpl->m_MinArm;
	}
	void MyCamera::SetMinArm(float f) {
		pImpl->m_MinArm = f;
	}

	float MyCamera::GetRotSpeed() const {
		return pImpl->m_RotSpeed;

	}
	void MyCamera::SetRotSpeed(float f) {
		pImpl->m_RotSpeed = abs(f);
	}

	bsm::Vec3 MyCamera::GetTargetToAt() const {
		return pImpl->m_TargetToAt;

	}
	void MyCamera::SetTargetToAt(const bsm::Vec3& v) {
		pImpl->m_TargetToAt = v;
	}

	bool MyCamera::GetLRBaseMode() const {
		return pImpl->m_LRBaseMode;

	}
	bool MyCamera::IsLRBaseMode() const {
		return pImpl->m_LRBaseMode;

	}
	void MyCamera::SetLRBaseMode(bool b) {
		pImpl->m_LRBaseMode = b;
	}
	bool MyCamera::GetUDBaseMode() const {
		return pImpl->m_UDBaseMode;

	}
	bool MyCamera::IsUDBaseMode() const {
		return pImpl->m_UDBaseMode;
	}
	void MyCamera::SetUDBaseMode(bool b) {
		pImpl->m_UDBaseMode = b;

	}

	void MyCamera::SetAt(const bsm::Vec3& At) {
		Camera::SetAt(At);
		bsm::Vec3 ArmVec = GetEye() - GetAt();
		ArmVec.normalize();
		ArmVec *= pImpl->m_ArmLen;
		bsm::Vec3 NewEye = GetAt() + ArmVec;
		SetEye(NewEye);
	}
	void MyCamera::SetAt(float x, float y, float z) {
		Camera::SetAt(x, y, z);
		bsm::Vec3 ArmVec = GetEye() - GetAt();
		ArmVec.normalize();
		ArmVec *= pImpl->m_ArmLen;
		bsm::Vec3 NewEye = GetAt() + ArmVec;
		SetEye(NewEye);

	}


	void MyCamera::OnUpdate() {
		auto CntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		auto KeyData = App::GetApp()->GetInputDevice().GetKeyState();
		//前回のターンからの時間
		float ElapsedTime = App::GetApp()->GetElapsedTime();
		bsm::Vec3 NewEye = GetEye();
		bsm::Vec3 NewAt = GetAt();
		//計算に使うための腕角度（ベクトル）
		bsm::Vec3 ArmVec = NewEye - NewAt;
		//正規化しておく
		ArmVec.normalize();
		if (CntlVec[0].bConnected) {
			if ((CntlVec[0].wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) || CntlVec[0].bLeftTrigger > 0.0f) {
				//カメラ位置を寄る
				pImpl->m_ArmLen -= pImpl->m_ZoomSpeed;
				//角度を変更
				pImpl->m_RadY -= 0.1f;
				if (pImpl->m_ArmLen <= pImpl->m_MinArm) {
					//m_MinArm以下近づかないようにする
					pImpl->m_ArmLen = pImpl->m_MinArm;
				}
				if (pImpl->m_RadY < 0.0f) {
					pImpl->m_RadY = 0.0f;
				}
			}
			else {
				//カメラ位置を引く
				pImpl->m_ArmLen += pImpl->m_ZoomSpeed;
				if (pImpl->m_ArmLen >= pImpl->m_MaxArm) {
					//m_MaxArm以上離れないようにする
					pImpl->m_ArmLen = pImpl->m_MaxArm;
				}

				//上下角度の変更
				if (CntlVec[0].fThumbRY >= 0.1f || KeyData.m_bPushKeyTbl[VK_UP]) {
					if (IsUDBaseMode()) {
						pImpl->m_RadY += pImpl->m_CameraUpDownSpeed * ElapsedTime;
					}
					else {
						pImpl->m_RadY -= pImpl->m_CameraUpDownSpeed * ElapsedTime;
					}
				}
				else if (CntlVec[0].fThumbRY <= -0.1f || KeyData.m_bPushKeyTbl[VK_DOWN]) {
					if (IsUDBaseMode()) {
						pImpl->m_RadY -= pImpl->m_CameraUpDownSpeed * ElapsedTime;
					}
					else {
						pImpl->m_RadY += pImpl->m_CameraUpDownSpeed * ElapsedTime;
					}
				}
				if (pImpl->m_RadY > XM_PI * 4 / 9.0f) {
					pImpl->m_RadY = XM_PI * 4 / 9.0f;
				}
				else if (pImpl->m_RadY <= pImpl->m_CameraUnderRot) {
					//カメラが限界下に下がったらそれ以上下がらない
					pImpl->m_RadY = pImpl->m_CameraUnderRot;
				}
			}
			ArmVec.y = sin(pImpl->m_RadY);
			//ここでY軸回転を作成
			if (CntlVec[0].fThumbRX != 0 || KeyData.m_bPushKeyTbl[VK_LEFT] || KeyData.m_bPushKeyTbl[VK_RIGHT]) {
				//回転スピードを反映
				if (CntlVec[0].fThumbRX != 0) {
					if (IsLRBaseMode()) {
						pImpl->m_RadXZ += -CntlVec[0].fThumbRX * ElapsedTime * pImpl->m_RotSpeed;
					}
					else {
						pImpl->m_RadXZ += CntlVec[0].fThumbRX * ElapsedTime * pImpl->m_RotSpeed;
					}
				}
				else if (KeyData.m_bPushKeyTbl[VK_LEFT]) {
					if (IsLRBaseMode()) {
						pImpl->m_RadXZ += ElapsedTime * pImpl->m_RotSpeed;
					}
					else {
						pImpl->m_RadXZ -= ElapsedTime * pImpl->m_RotSpeed;
					}
				}
				else if (KeyData.m_bPushKeyTbl[VK_RIGHT]) {
					if (IsLRBaseMode()) {
						pImpl->m_RadXZ -= ElapsedTime * pImpl->m_RotSpeed;
					}
					else {
						pImpl->m_RadXZ += ElapsedTime * pImpl->m_RotSpeed;
					}

				}
				if (abs(pImpl->m_RadXZ) >= XM_2PI) {
					//1週回ったら0回転にする
					pImpl->m_RadXZ = 0;
				}
			}
			//クオータニオンでY回転（つまりXZベクトルの値）を計算
			bsm::Quat QtXZ;
			QtXZ.rotation(pImpl->m_RadXZ, bsm::Vec3(0, 1.0f, 0));
			QtXZ.normalize();
			//移動先行の行列計算することで、XZの値を算出
			bsm::Mat4x4 Mat;
			Mat.strTransformation(
				bsm::Vec3(1.0f, 1.0f, 1.0f),
				bsm::Vec3(0.0f, 0.0f, -1.0f),
				QtXZ
			);

			bsm::Vec3 PosXZ = Mat.transInMatrix();
			//XZの値がわかったので腕角度に代入
			ArmVec.x = PosXZ.x;
			ArmVec.z = PosXZ.z;
			//腕角度を正規化
			ArmVec.normalize();

			auto TargetPtr = GetTargetObject();
			if (TargetPtr) {
				//目指したい場所
				bsm::Vec3 ToAt = TargetPtr->GetComponent<Transform>()->GetWorldMatrix().transInMatrix();
				ToAt += pImpl->m_TargetToAt;
				NewAt = Lerp::CalculateLerp(GetAt(), ToAt, 0, 1.0f, 1.0, Lerp::Linear);
			}

			////目指したい場所にアームの値と腕ベクトルでEyeを調整
			bsm::Vec3 ToEye = NewAt + ArmVec * pImpl->m_ArmLen;
			NewEye = Lerp::CalculateLerp(GetEye(), ToEye, 0, 1.0f, pImpl->m_ToTargetLerp, Lerp::Linear);
		}
		if (KeyData.m_bPressedKeyTbl[VK_LEFT]) {
			int a = 0;
		}

		SetAt(NewAt);
		SetEye(NewEye);
		UpdateArmLengh();
		Camera::OnUpdate();
	}

	//--------------------------------------------------------------------------------------
	///	プレイヤーの行動クラス
	//--------------------------------------------------------------------------------------
	Vec3 PlayerBehavior::GetMoveVector() const {
		Vec3 Angle(0, 0, 0);
		//コントローラの取得
		auto CntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		if (CntlVec[0].bConnected) {
			if (CntlVec[0].fThumbLX != 0 || CntlVec[0].fThumbLY != 0) {
				float MoveLength = 0;	//動いた時のスピード
				auto PtrTransform = GetGameObject()->GetComponent<Transform>();
				auto PtrCamera = GetGameObject()->OnGetDrawCamera();
				//進行方向の向きを計算
				Vec3 Front = PtrTransform->GetWorldMatrix().transInMatrix() - PtrCamera->GetEye();
				Front.y = 0;
				Front.normalize();
				//進行方向向きからの角度を算出
				float FrontAngle = atan2(Front.z, Front.x);
				//コントローラの向き計算
				float MoveX = CntlVec[0].fThumbLX;
				float MoveZ = CntlVec[0].fThumbLY;
				Vec2 MoveVec(MoveX, MoveZ);
				float MoveSize = MoveVec.length();
				//コントローラの向きから角度を計算
				float CntlAngle = atan2(-MoveX, MoveZ);
				//トータルの角度を算出
				float TotalAngle = FrontAngle + CntlAngle;
				//角度からベクトルを作成
				Angle = Vec3(cos(TotalAngle), 0, sin(TotalAngle));
				//正規化する
				Angle.normalize();
				//移動サイズを設定。
				Angle *= MoveSize;
				//Y軸は変化させない
				Angle.y = 0;
			}
		}
		return Angle;
	}

	void PlayerBehavior::MovePlayer() {
		float ElapsedTime = App::GetApp()->GetElapsedTime();
		Vec3 Angle = GetMoveVector();
		//Rigidbodyを取り出す
		auto PtrRedit = GetGameObject()->GetComponent<Rigidbody>();
		auto Velo = PtrRedit->GetVelocity();
		if (Angle.length() <= 0.0f && Velo.y == 0.0f) {
			//コントローラを離したとき対策
			Velo *= GetDecel();
			PtrRedit->SetVelocity(Velo);
			return;
		}
		//Transform
		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		//現在の速度を取り出す
		//目的地を最高速度を掛けて求める
		auto Target = Angle * GetMaxSpeed();
		//目的地に向かうために力のかける方向を計算する
		//Forceはフォースである
		auto Force = Target - Velo;
		//yは0にする
		Force.y = 0;
		//加速度を求める
		auto Accel = Force / GetMass();
		//ターン時間を掛けたものを速度に加算する
		Velo += (Accel * ElapsedTime);
		//速度を設定する
		PtrRedit->SetVelocity(Velo);
		//回転の計算
		if (Angle.length() > 0.0f) {
			auto UtilPtr = GetGameObject()->GetBehavior<UtilBehavior>();
			UtilPtr->RotToHead(Angle, 1.0f);
		}
	}

	void PlayerBehavior::FireAttackBall() {
		auto PtrCamera = GetGameObject()->OnGetDrawCamera();
		auto PtrTrans = GetGameObject()->GetComponent<Transform>();
		auto Group = GetStage()->GetSharedObjectGroup(L"AttackBall");
		for (auto& v : Group->GetGroupVector()) {
			auto shptr = v.lock();
			if (shptr) {
				auto AttackPtr = dynamic_pointer_cast<AttackBall>(shptr);
				if (AttackPtr && !AttackPtr->IsUpdateActive()) {
					//回転の計算
					auto RotY = PtrTrans->GetRotation().y;
					auto Angle = Vec3(sin(RotY), 0, cos(RotY));

					auto CntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
					if (CntlVec[0].bConnected) {
						if (CntlVec[0].wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
							//カメラを得る
							auto PtrCamera = dynamic_pointer_cast<MyCamera>(GetGameObject()->OnGetDrawCamera());
							if (PtrCamera) {
								Vec3 CameraPos = PtrCamera->GetEye();
								Vec3 MyPos = PtrTrans->GetPosition();
								Vec3 ShootVec = MyPos - CameraPos;
								ShootVec.y = 0;
								Angle = ShootVec;
							}
						}
					}

					Angle.normalize();
					auto Span = Angle * 0.5f;
					AttackPtr->Weakup(PtrTrans->GetWorldMatrix().transInMatrix() + Span, Angle * 40.0f);
					return;
				}
			}
		}
	}

	bool PlayerBehavior::OnHitObjMoveBox(vector<shared_ptr<GameObject>>& OtherVec, shared_ptr<GameObject>& retv) {
		for (auto& v : OtherVec) {
			if (v->FindTag(L"MoveBox")) {
				auto PtrTrans = GetGameObject()->GetComponent<Transform>();
				auto sp = GetGameObject()->GetComponent<CollisionSphere>()->GetSphere();
				auto obb = v->GetComponent<CollisionObb>()->GetObb();
				Vec3 RetPoint;
				HitTest::SPHERE_OBB(sp, obb, RetPoint);
				auto Normal = sp.m_Center - RetPoint;
				if (Normal.length() > sp.m_Radius * 1.5f) {
					return false;
				}
				Normal.normalize();
				Vec3 angle(XMVector3AngleBetweenNormals(Vec3(0, 1.0f, 0), Normal));
				if (angle.x >= 0.1f) {
					return false;
				}
				retv = v;
				return true;
			}
		}
		return false;
	}


	bool PlayerBehavior::OnMoveBox() {
		auto PtrTrans = GetGameObject()->GetComponent<Transform>();
		auto sp = GetGameObject()->GetComponent<CollisionSphere>()->GetSphere();
		auto obb = PtrTrans->GetParent()->GetComponent<CollisionObb>()->GetObb();
		Vec3 RetPoint;
		HitTest::SPHERE_OBB(sp, obb, RetPoint);
		auto Normal = sp.m_Center - RetPoint;
		if (Normal.length() > sp.m_Radius * 1.5f) {
			return false;
		}
		Normal.normalize();
		Vec3 angle(XMVector3AngleBetweenNormals(Vec3(0, 1.0f, 0), Normal));
		if (angle.x >= 0.1f) {
			return false;
		}
		return true;
	}

}

//end basecross

