/*!
@file GameStage.h
@brief ƒQ[ƒ€ƒXƒe[ƒW
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	//--------------------------------------------------------------------------------------
	//	ƒQ[ƒ€ƒXƒe[ƒWƒNƒ‰ƒX
	//--------------------------------------------------------------------------------------
	class GameStage : public Stage {
		//ƒrƒ…[‚Æƒ‰ƒCƒg‚Ìì¬
		void CreateViewLight();
		//ƒvƒŒ[ƒg‚Ìì¬
		void CreatePlate();
		//ƒvƒŒƒCƒ„[‚Ìì¬
		void CreatePlayer();
		//“G‚Ìì¬
		void CreateEnemy();
		//ï¿½^ï¿½[ï¿½Qï¿½bï¿½gï¿½Ìì¬
		void CreateTarget();
	public:
		//\’z‚Æ”jŠü
		GameStage() :Stage() {}
		virtual ~GameStage() {}
		//‰Šú‰»
		virtual void OnCreate()override;
	};


}
//end basecross

