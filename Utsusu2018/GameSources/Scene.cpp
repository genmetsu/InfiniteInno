
/*!
@file Scene.cpp
@brief シーン実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross{

	//--------------------------------------------------------------------------------------
	///	ゲームシーン
	//--------------------------------------------------------------------------------------
	void Scene::CreateResourses() {
		wstring DataDir;
		//サンプルのためアセットディレクトリを取得
		App::GetApp()->GetAssetsDirectory(DataDir);
		//各ゲームは以下のようにデータディレクトリを取得すべき
		//App::GetApp()->GetDataDirectory(DataDir);
		wstring strTexture = DataDir + L"trace.png";
		App::GetApp()->RegisterTexture(L"TRACE_TX", strTexture);
		strTexture = DataDir + L"sky.jpg";
		App::GetApp()->RegisterTexture(L"SKY_TX", strTexture);
		strTexture = DataDir + L"wall.jpg";
		App::GetApp()->RegisterTexture(L"WALL_TX", strTexture);
		strTexture = DataDir + L"spark.png";
		App::GetApp()->RegisterTexture(L"SPARK_TX", strTexture);
		strTexture = DataDir + L"fire.png";
		App::GetApp()->RegisterTexture(L"FIRE_TX", strTexture);
		strTexture = DataDir + L"number.png";
		App::GetApp()->RegisterTexture(L"NUMBER_TX", strTexture);

		strTexture = DataDir + L"grass256.png";
		App::GetApp()->RegisterTexture(L"GRASS_TX", strTexture);
		strTexture = DataDir + L"Red.png";
		App::GetApp()->RegisterTexture(L"RED_TX", strTexture);

		//プレート用
		strTexture = DataDir + L"gray.png";
		App::GetApp()->RegisterTexture(L"GRAY_TX", strTexture);
		strTexture = DataDir + L"normal3.png";
		App::GetApp()->RegisterTexture(L"NORMAL3_TX", strTexture);

		//以下オーディオ

		//サウンド
		wstring CursorWav = DataDir + L"cursor.wav";
		App::GetApp()->RegisterWav(L"cursor", CursorWav);

		//BGM
		wstring strMusic = DataDir + L"nanika .wav";
		App::GetApp()->RegisterWav(L"Nanika", strMusic);

	}

	void Scene::OnCreate() {
		try {
			//リソースの作成
			CreateResourses();

			//BGMの再生
			auto XAPtr = App::GetApp()->GetXAudio2Manager();
			m_AudioObjectPtr = XAPtr->Start(L"Nanika", XAUDIO2_LOOP_INFINITE, 0.1f);

			//クリアする色を設定
			Col4 Col;
			Col.set(180.0f / 255.0f,210.0f / 255.0f,255.0f / 255.0f,255.0f / 255.0f);
			SetClearColor(Col);
			//自分自身にイベントを送る
			//これにより各ステージやオブジェクトがCreate時にシーンにアクセスできる
			PostEvent(0.0f, GetThis<ObjectInterface>(), GetThis<Scene>(), L"ToGameStage");
		}
		catch (...) {
			throw;
		}
	}

	void Scene::OnEvent(const shared_ptr<Event>& event) {
		if (event->m_MsgStr == L"ToGameStage") {
			//最初のアクティブステージの設定
			ResetActiveStage<GameStage>();
		}
	}

	void Scene::OnDestroy() {
		//親クラスのOnDestroyを呼ぶ
		SceneBase::OnDestroy();

		auto XAPtr = App::GetApp()->GetXAudio2Manager();
		XAPtr->Stop(m_AudioObjectPtr);

	}

}
//end basecross
