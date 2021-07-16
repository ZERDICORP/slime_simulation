#include "tools.h"

float sense(zer::row<float>& fPhemap, float fY, float fX, float fAngle, float fSensorAngleOffset, float fSensorLength, float fSensorDistance)
{
	float fSensorAngle = fAngle + fSensorAngleOffset;
	float fSummaryWeight = 0;

	for (int i = 0; i < fSensorLength; ++i)
	{
		int iTempY = fY + sin(fSensorAngle) * (i + 1 + fSensorDistance);
		int iTempX = fX + cos(fSensorAngle) * (i + 1 + fSensorDistance);

		if (zer::athm::pointInside2dMatrix(mWH, mWW, iTempY, iTempX))
			fSummaryWeight += fPhemap[mWW * iTempY + iTempX];
	}

	return fSummaryWeight;
}

void setPixelToPixmap(zer::row<uint8_t>& ui8Pixmap, int iIndex, sf::Color& cColor)
{
	ui8Pixmap[iIndex * 4 + 0] = cColor.r;
	ui8Pixmap[iIndex * 4 + 1] = cColor.g;
	ui8Pixmap[iIndex * 4 + 2] = cColor.b;
	ui8Pixmap[iIndex * 4 + 3] = cColor.a;
}

void setPixelToPixmap(zer::row<uint8_t>& ui8Pixmap, int iIndex, sf::Color&& cColor)
{
	setPixelToPixmap(ui8Pixmap, iIndex, cColor);
}

void displayConsoleInformation(zer::dict<std::string, float>& dCfg, bool bSimulationStarted)
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
	for (int i = 0; i < dCfg.len(); ++i)
		std::cout << "\t" << dCfg.key(i) << " = " << dCfg[dCfg.key(i)] << ";" << std::endl;
}

zer::dict<std::string, float> readConfig(std::string sConfigPath)
{
	zer::dict<std::string, float> d;

	zer::File file(sConfigPath);
	file.read({zer::file::Modifier::lines});

	for (int i = 0; i < file.linesLen(); ++i)
	{
		std::string sLine = file.line(i);
		if (zer::str::have(sLine, " = "))
		{
			zer::row<string> lineParts = zer::str::split(sLine, " = ");
			d.add(lineParts[0], stof(lineParts[1]));
		}
	}

	return d;
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