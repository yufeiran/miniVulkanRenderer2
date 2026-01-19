#pragma once
#include<unordered_map>
#include<string>

namespace mini
{

	class MiniSetting
	{
	public:
		bool ssr_on = false;
		bool bloom_on = false;
		bool light_cube = false;

		inline void set(const std::string& key, const std::string& value)
		{
			settings_[key] = value;
		}
		
		inline void get(const std::string& key)const
		{
			auto it = settings_.find(key);
		}
	private:
		std::unordered_map<std::string, std::string> settings_;
	};


	MiniSetting& getGlobalMiniSetting();
}
