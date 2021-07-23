#include "config.h"
#include "tools.h"
#include "agent.h"

int loop(sf::RenderWindow& window, std::map<std::string, float>& cfg)
{
	bool bSimulationStarted = false;
	bool bNeedToUpdateConsole = true;

	float fPheromoneDisappearanceRate = cfg["pheromoneDisappearanceRate"];
	float fPheromoneConcentration = cfg["pheromoneConcentration"];
	float fSensorLength = cfg["sensorLength"];
	float fSensorDistance = cfg["sensorDistance"];
	float fSensorAngleOffset = zer::athm::toRadians(cfg["sensorAngleOffset"]);
	float fItemTurnSpeed = cfg["itemTurnSpeed"];
	float fItemSpeed = cfg["itemSpeed"];

	std::vector<uint8_t> pixmap(mWH * mWW * 4);
	for (int i = 0; i < mWH * mWW; ++i)
		setPixelToPixmap(pixmap, i, sf::Color(0, 0, 0));

	std::vector<float> phemap(mWH * mWW, 0);

	std::vector<Agent> agents;
	
	int iRows = sqrt(cfg["itemsQuantity"]);
	int iCols = iRows;
	int iSqW = ceil((float)mWW / iCols);

	for (int iRow = 1; iRow < iRows - 2; ++iRow)
		for (int iCol = 1; iCol < iCols - 2; ++iCol)
		{
			int y = iRow * iSqW;
			int x = iCol * iSqW;

			if (y >= mWH)
				y = y - mWH;
			if (x >= mWW)
				x = x - mWW;
			
			agents.push_back(Agent(y, x, zer::athm::getAngleBetweenPoints(y, x, mWH / 2, mWW / 2)));
			setPixelToPixmap(pixmap, mWW * y + x, sf::Color(255, 255, 255));
		}

	sf::Texture texture;
	texture.create(mWW, mWH);

	/*
		Just to make the code more readable and simpler.
	*/
	auto senseWrapper = [&phemap, &fSensorLength, &fSensorDistance](Agent& agent, float fSensorAngleOffset) -> float
	{
		return sense(phemap, agent.fY, agent.fX, agent.fAngle, fSensorAngleOffset, fSensorLength, fSensorDistance);
	};

	while (window.isOpen())
	{
		window.clear();

		if (bSimulationStarted)
		{
			for (int i = 0; i < mWH * mWW; ++i)
				setPixelToPixmap(pixmap, i, sf::Color(0, 0, 0));

			/*
				Drawing a pheromone map.
			*/
			for (int i = 0; i < mWH * mWW; ++i)
			{
				if (phemap[i])
				{
					phemap[i] *= (1 - fPheromoneDisappearanceRate);
					if (phemap[i] < 1)
						phemap[i] = 0;

					int iColorValue = phemap[i];
					if (iColorValue > 255)
						iColorValue = 255;

					setPixelToPixmap(pixmap, i, sf::Color(iColorValue, iColorValue, iColorValue));
				}
			}

			/*
				Agents movement logic.
			*/
			for (int i = 0; i < agents.size(); ++i)
			{
				phemap[mWW * (int)agents[i].fY + (int)agents[i].fX] += fPheromoneConcentration;

				float fWeightForward = senseWrapper(agents[i], 0);
				float fWeightLeft = senseWrapper(agents[i], fSensorAngleOffset);
				float fWeightRight = senseWrapper(agents[i], -fSensorAngleOffset);
				float fRandomAmplifier = zer::athm::rand_float();

				if (fWeightForward > fWeightLeft && fWeightForward > fWeightRight)
					agents[i].fAngle += 0;
				else if (fWeightForward < fWeightLeft && fWeightForward < fWeightRight)
					agents[i].fAngle += (fRandomAmplifier - 0.5) * 2 * fItemTurnSpeed;
				else if (fWeightLeft > fWeightRight)
					agents[i].fAngle += fRandomAmplifier * fItemTurnSpeed;
				else if (fWeightRight > fWeightLeft)
					agents[i].fAngle -= fRandomAmplifier * fItemTurnSpeed;
				else
					agents[i].fAngle += zer::athm::toRadians(zer::athm::rand_int(-10, 10));

				float fNextAgentY = agents[i].fY + sin(agents[i].fAngle) * fItemSpeed;
				float fNextAgentX = agents[i].fX + cos(agents[i].fAngle) * fItemSpeed;
				
				if (zer::athm::inRange2D(mWH, mWW, fNextAgentY, fNextAgentX))
				{
					agents[i].fY = fNextAgentY;
					agents[i].fX = fNextAgentX;
				}
				else
					agents[i].fAngle += zer::athm::toRadians(180);
			}
		}

		texture.update(&pixmap[0]);
		window.draw(sf::Sprite(texture));

		window.display();

		if (bNeedToUpdateConsole)
		{
			displayConsoleInformation(cfg, bSimulationStarted);
			bNeedToUpdateConsole = false;
		}

		switch (eventListener(window))
		{
			case (CLOSE_EVENT_CODE):
				window.close();
				break;

			case START_EVENT_CODE:
				bNeedToUpdateConsole = true;
				bSimulationStarted = !bSimulationStarted;
				break;

			case RESTART_EVENT_CODE:
				init(window);
				break;
		}
	}

	return 0;
}

int init(sf::RenderWindow& window)
{
	zer::athm::rand_init();

	std::map<std::string, float> cfg = readConfig(msConfigPath);
	
	return loop(window, cfg);
}