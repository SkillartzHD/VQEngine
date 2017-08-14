#pragma once

#include <vector>
#include <string>

#include "Settings.h"
#include "SceneManager.h"

class SceneParser
{
public:
	SceneParser();
	~SceneParser();

	static Settings::Window ReadWindowSettings();
	static void ParseSetting(const std::vector<std::string>& line, Settings::Window& settings);

	static SerializedScene ReadScene();
	static void ParseScene(const std::vector<std::string>& command, SerializedScene& scene);
private:

};

