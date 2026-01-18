#include"miniSetting.h"


namespace mini
{
	MiniSetting& getGlobalMiniSetting()
	{
		static MiniSetting globalMiniSetting;
		return globalMiniSetting;
	}

}
