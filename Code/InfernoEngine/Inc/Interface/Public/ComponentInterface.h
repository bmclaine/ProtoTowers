#pragma once
//#include "InfernoEngine.h"

class InfernoEngine;

// *** Currently breaking due to include paths ****

namespace InfernoEngine_Components
{

	class ComponentInterface
	{
	private:
		// === Static Variables
		static InfernoEngine* s_pEngineInstance;

	public:
		static void SetEngineInstance(InfernoEngine* _engine) {
			s_pEngineInstance = _engine;
		}

		template <typename type>
		static unsigned int RegisterComponent(const char* _componentID) {
			//return s_pEngineInstance->RegisterComponent<type>(_componentID);
			return 0;
		}

		template <typename type>
		static unsigned int RegisterUIComponent(const char* _uiComponentID) {
			//return s_pEngineInstance->RegisterUIComponent<type>(_uiComponentID);
			return 0;
		}

	};

}

