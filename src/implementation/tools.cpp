#include "tools.h"

float sense(std::vector<float>& phemap, float fY, float fX, float fAngle, float fSensorAngleOffset, float fSensorLength, float fSensorDistance)
{
	float fSensorAngle = fAngle + fSensorAngleOffset;
	float fSummaryWeight = 0;

	for (int i = 0; i < fSensorLength; ++i)
	{
		int iTempY = fY + sin(fSensorAngle) * (i + 1 + fSensorDistance);
		int iTempX = fX + cos(fSensorAngle) * (i + 1 + fSensorDistance);

		if (zer::athm::inRange2D(mWH, mWW, iTempY, iTempX))
			fSummaryWeight += phemap[mWW * iTempY + iTempX];
	}

	return fSummaryWeight;
}

void setPixelToPixmap(std::vector<uint8_t>& pixmap, int iIndex, sf::Color& cColor)
{
	pixmap[iIndex * 4 + 0] = cColor.r;
	pixmap[iIndex * 4 + 1] = cColor.g;
	pixmap[iIndex * 4 + 2] = cColor.b;
	pixmap[iIndex * 4 + 3] = cColor.a;
}

void setPixelToPixmap(std::vector<uint8_t>& pixmap, int iIndex, sf::Color&& cColor)
{
	setPixelToPixmap(pixmap, iIndex, cColor);
}

void displayConsoleInformation(std::map<std::string, float>& cfg, bool bSimulationStarted)
{
	system("cls");

	std::cout << "# " << msTitle << " #" << std::endl;
	std::cout << "\n[!] simulation state: " << (bSimulationStarted ? "STARTED" : "STOPPED") << std::endl;
	std::cout << "\n[!] keyboard buttons for control:" << std::endl;
	std::cout << "\t [ ESC ] - exit;" << std::endl;
	std::cout << "\t [ SPACE ] - start/stop simulation;" << std::endl;
	std::cout << "\t [ R ] - restart simulation;" << std::endl;
	std::cout << "\n[!] note: visit a \"" << msConfigPath << "\" file to change simulation configuration;" << std::endl;
	std::cout << "\n[!] current configuration:" << std::endl;
	
	for (std::map<std::string, float>::iterator p = cfg.begin(); p != cfg.end(); p++)
		std::cout << "\t" << p -> first << " = " << p -> second << ";" << std::endl;
}

std::map<std::string, float> readConfig(std::string sConfigPath)
{
	std::map<std::string, float> cfg;

	zer::File file(sConfigPath);
	file.read({zer::file::Modifier::lines});

	for (int i = 0; i < file.linesLen(); ++i)
	{
		std::string sLine = file.lineAt(i);
		if (sLine.find(" = ") != std::string::npos)
		{
			std::vector<std::string> lineParts = zer::athm::split(sLine, " = ");
			cfg[lineParts[0]] = stof(lineParts[1]);
		}
	}

	return cfg;
}

EVENT_CODE eventListener(sf::RenderWindow& window)
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			return CLOSE_EVENT_CODE;
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
				return CLOSE_EVENT_CODE;
			if (event.key.code == sf::Keyboard::Space)
				return START_EVENT_CODE;
			if (event.key.code == sf::Keyboard::R)
				return RESTART_EVENT_CODE;
		}
	}
	return NULL_EVENT_CODE;
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(mWW, mWH), msTitle);
	return init(window);
}