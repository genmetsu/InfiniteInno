/*!
@file ProjectBehavior.cpp
@brief �v���W�F�N��`�̍s���N���X����
*/

#include "stdafx.h"
#include "Project.h"


namespace basecross {

	//--------------------------------------------------------------------------------------
	//	struct MyCamera::Impl;
	//	�p�r: Impl�N���X
	//--------------------------------------------------------------------------------------
	struct MyCamera::Impl {
		weak_ptr<GameObject> m_TargetObject;	//�ڕW�ƂȂ�I�u�W�F�N�g
		float m_ToTargetLerp;	//�ڕW��ǂ�������ۂ̕�Ԓl
		bsm::Vec3 m_TargetToAt;	//�ڕW���王�_�𒲐�����ʒu�x�N�g��

		float m_RadY;
		float m_RadXZ;
		//�J�����̏㉺�X�s�[�h
		float m_CameraUpDownSpeed;
		//�J�����������鉺���p�x
		float m_CameraUnderRot;
		//�r�̒����̐ݒ�
		float m_ArmLen;
		float m_MaxArm;
		float m_MinArm;
		//��]�X�s�[�h
		float m_RotSpeed;
		//�Y�[���X�s�[�h
		float m_ZoomSpeed;
		//���E�X�e�B�b�N�ύX�̃��[�h
		bool m_LRBaseMode;
		//�㉺�X�e�B�b�N�ύX�̃��[�h
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
	//	�p�r: MyCamera�i�R���|�[�l���g�ł͂Ȃ��j
	//--------------------------------------------------------------------------------------
	//�\�z�Ɣj��
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
	//�A�N�Z�T
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
			//m_MaxArm�ȏ㗣��Ȃ��悤�ɂ���
			pImpl->m_ArmLen = pImpl->m_MaxArm;
		}
		if (pImpl->m_ArmLen <= pImpl->m_MinArm) {
			//m_MinArm�ȉ��߂Â��Ȃ��悤�ɂ���
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
		//�O��̃^�[������̎���
		float ElapsedTime = App::GetApp()->GetElapsedTime();
		bsm::Vec3 NewEye = GetEye();
		bsm::Vec3 NewAt = GetAt();
		//�v�Z�Ɏg�����߂̘r�p�x�i�x�N�g���j
		bsm::Vec3 ArmVec = NewEye - NewAt;
		//���K�����Ă���
		ArmVec.normalize();
		if (CntlVec[0].bConnected) {
			if ((CntlVec[0].wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) || CntlVec[0].bLeftTrigger > 0.0f) {
				//�J�����ʒu�����
				pImpl->m_ArmLen -= pImpl->m_ZoomSpeed;
				//�p�x��ύX
				pImpl->m_RadY -= 0.1f;
				if (pImpl->m_ArmLen <= pImpl->m_MinArm) {
					//m_MinArm�ȉ��߂Â��Ȃ��悤�ɂ���
					pImpl->m_ArmLen = pImpl->m_MinArm;
				}
				if (pImpl->m_RadY < 0.0f) {
					pImpl->m_RadY = 0.0f;
				}
			}
			else {
				//�J�����ʒu������
				pImpl->m_ArmLen += pImpl->m_ZoomSpeed;
				if (pImpl->m_ArmLen >= pImpl->m_MaxArm) {
					//m_MaxArm�ȏ㗣��Ȃ��悤�ɂ���
					pImpl->m_ArmLen = pImpl->m_MaxArm;
				}

				//�㉺�p�x�̕ύX
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
					//�J���������E���ɉ��������炻��ȏ㉺����Ȃ�
					pImpl->m_RadY = pImpl->m_CameraUnderRot;
				}
			}
			ArmVec.y = sin(pImpl->m_RadY);
			//������Y����]���쐬
			if (CntlVec[0].fThumbRX != 0 || KeyData.m_bPushKeyTbl[VK_LEFT] || KeyData.m_bPushKeyTbl[VK_RIGHT]) {
				//��]�X�s�[�h�𔽉f
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
					//1�T�������0��]�ɂ���
					pImpl->m_RadXZ = 0;
				}
			}
			//�N�I�[�^�j�I����Y��]�i�܂�XZ�x�N�g���̒l�j���v�Z
			bsm::Quat QtXZ;
			QtXZ.rotation(pImpl->m_RadXZ, bsm::Vec3(0, 1.0f, 0));
			QtXZ.normalize();
			//�ړ���s�̍s��v�Z���邱�ƂŁAXZ�̒l���Z�o
			bsm::Mat4x4 Mat;
			Mat.strTransformation(
				bsm::Vec3(1.0f, 1.0f, 1.0f),
				bsm::Vec3(0.0f, 0.0f, -1.0f),
				QtXZ
			);

			bsm::Vec3 PosXZ = Mat.transInMatrix();
			//XZ�̒l���킩�����̂Řr�p�x�ɑ��
			ArmVec.x = PosXZ.x;
			ArmVec.z = PosXZ.z;
			//�r�p�x�𐳋K��
			ArmVec.normalize();

			auto TargetPtr = GetTargetObject();
			if (TargetPtr) {
				//�ڎw�������ꏊ
				bsm::Vec3 ToAt = TargetPtr->GetComponent<Transform>()->GetWorldMatrix().transInMatrix();
				ToAt += pImpl->m_TargetToAt;
				NewAt = Lerp::CalculateLerp(GetAt(), ToAt, 0, 1.0f, 1.0, Lerp::Linear);
			}

			////�ڎw�������ꏊ�ɃA�[���̒l�Ƙr�x�N�g����Eye�𒲐�
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
	///	�v���C���[�̍s���N���X
	//--------------------------------------------------------------------------------------
	Vec3 PlayerBehavior::GetMoveVector() const {
		Vec3 Angle(0, 0, 0);
		//�R���g���[���̎擾
		auto CntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		if (CntlVec[0].bConnected) {
			if (CntlVec[0].fThumbLX != 0 || CntlVec[0].fThumbLY != 0) {
				float MoveLength = 0;	//���������̃X�s�[�h
				auto PtrTransform = GetGameObject()->GetComponent<Transform>();
				auto PtrCamera = GetGameObject()->OnGetDrawCamera();
				//�i�s�����̌������v�Z
				Vec3 Front = PtrTransform->GetWorldMatrix().transInMatrix() - PtrCamera->GetEye();
				Front.y = 0;
				Front.normalize();
				//�i�s������������̊p�x���Z�o
				float FrontAngle = atan2(Front.z, Front.x);
				//�R���g���[���̌����v�Z
				float MoveX = CntlVec[0].fThumbLX;
				float MoveZ = CntlVec[0].fThumbLY;
				Vec2 MoveVec(MoveX, MoveZ);
				float MoveSize = MoveVec.length();
				//�R���g���[���̌�������p�x���v�Z
				float CntlAngle = atan2(-MoveX, MoveZ);
				//�g�[�^���̊p�x���Z�o
				float TotalAngle = FrontAngle + CntlAngle;
				//�p�x����x�N�g�����쐬
				Angle = Vec3(cos(TotalAngle), 0, sin(TotalAngle));
				//���K������
				Angle.normalize();
				//�ړ��T�C�Y��ݒ�B
				Angle *= MoveSize;
				//Y���͕ω������Ȃ�
				Angle.y = 0;
			}
		}
		return Angle;
	}

	void PlayerBehavior::MovePlayer() {
		float ElapsedTime = App::GetApp()->GetElapsedTime();
		Vec3 Angle = GetMoveVector();
		//Rigidbody�����o��
		auto PtrRedit = GetGameObject()->GetComponent<Rigidbody>();
		auto Velo = PtrRedit->GetVelocity();
		if (Angle.length() <= 0.0f && Velo.y == 0.0f) {
			//�R���g���[���𗣂����Ƃ��΍�
			Velo *= GetDecel();
			PtrRedit->SetVelocity(Velo);
			return;
		}
		//Transform
		auto PtrTransform = GetGameObject()->GetComponent<Transform>();
		//���݂̑��x�����o��
		//�ړI�n���ō����x���|���ċ��߂�
		auto Target = Angle * GetMaxSpeed();
		//�ړI�n�Ɍ��������߂ɗ͂̂�����������v�Z����
		//Force�̓t�H�[�X�ł���
		auto Force = Target - Velo;
		//y��0�ɂ���
		Force.y = 0;
		//�����x�����߂�
		auto Accel = Force / GetMass();
		//�^�[�����Ԃ��|�������̂𑬓x�ɉ��Z����
		Velo += (Accel * ElapsedTime);
		//���x��ݒ肷��
		PtrRedit->SetVelocity(Velo);
		//��]�̌v�Z
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
					//��]�̌v�Z
					auto RotY = PtrTrans->GetRotation().y;
					auto Angle = Vec3(sin(RotY), 0, cos(RotY));

					auto CntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
					if (CntlVec[0].bConnected) {
						if (CntlVec[0].wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
							//�J�����𓾂�
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

