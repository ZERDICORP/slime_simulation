#include "config.h"
#include "tools.h"
#include "agent.h"

int loop(sf::RenderWindow& window, zer::dict<std::string, float>& dCfg)
{
	bool bSimulationStarted = false;
	bool bNeedToUpdateConsole = true;

	float fPheromoneDisappearanceRate = dCfg["pheromoneDisappearanceRate"];
	float fPheromoneConcentration = dCfg["pheromoneConcentration"];
	float fSensorLength = dCfg["sensorLength"];
	float fSensorDistance = dCfg["sensorDistance"];
	float fSensorAngleOffset = zer::athm::toRadians(dCfg["sensorAngleOffset"]);
	float fItemTurnSpeed = dCfg["itemTurnSpeed"];
	float fItemSpeed = dCfg["itemSpeed"];

	zer::row<uint8_t> ui8Pixmap(mWH * mWW * 4);
	for (int i = 0; i < mWH * mWW; ++i)
		setPixelToPixmap(ui8Pixmap, i, sf::Color(0, 0, 0));

	zer::row<float> fPhemap(mWH * mWW, 0);

	zer::row<Agent> agents;
	
	int iRows = sqrt(dCfg["itemsQuantity"]);
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
			
			agents.end(Agent(y, x, zer::athm::getAngleToPoint(y, x, mWH / 2, mWW / 2)));
			setPixelToPixmap(ui8Pixmap, mWW * y + x, sf::Color(255, 255, 255));
		}

	sf::Texture texture;
	texture.create(mWW, mWH);

	/*
		Just to make the code more readable and simpler.
	*/
	auto senseWrapper = [&fPhemap, &fSensorLength, &fSensorDistance](Agent& agent, float fSensorAngleOffset) -> float
	{
		return sense(fPhemap, agent.fY, agent.fX, agent.fAngle, fSensorAngleOffset, fSensorLength, fSensorDistance);
	};

	while (window.isOpen())
	{
		window.clear();

		if (bSimulationStarted)
		{
			for (int i = 0; i < mWH * mWW; ++i)
				setPixelToPixmap(ui8Pixmap, i, sf::Color(0, 0, 0));

			/*
				Drawing a pheromone map.
			*/
			for (int i = 0; i < mWH * mWW; ++i)
			{
				if (fPhemap[i])
				{
					fPhemap[i] *= (1 - fPheromoneDisappearanceRate);
					if (fPhemap[i] < 1)
						fPhemap[i] = 0;

					int iColorValue = fPhemap[i];
					if (iColorValue > 255)
						iColorValue = 255;

					setPixelToPixmap(ui8Pixmap, i, sf::Color(iColorValue, iColorValue, iColorValue));
				}
			}

			/*
				Agents movement logic.
			*/
			for (int i = 0; i < agents.len(); ++i)
			{
				fPhemap[mWW * (int)agents[i].fY + (int)agents[i].fX] += fPheromoneConcentration;

				float fWeightForward = senseWrapper(agents[i], 0);
				float fWeightLeft = senseWrapper(agents[i], fSensorAngleOffset);
				float fWeightRight = senseWrapper(agents[i], -fSensorAngleOffset);
				float fRandomAmplifier = zer::rnd::randf();

				if (fWeightForward > fWeightLeft && fWeightForward > fWeightRight)
					agents[i].fAngle += 0;
				else if (fWeightForward < fWeightLeft && fWeightForward < fWeightRight)
					agents[i].fAngle += (fRandomAmplifier - 0.5) * 2 * fItemTurnSpeed;
				else if (fWeightLeft > fWeightRight)
					agents[i].fAngle += fRandomAmplifier * fItemTurnSpeed;
				else if (fWeightRight > fWeightLeft)
					agents[i].fAngle -= fRandomAmplifier * fItemTurnSpeed;
				else
					agents[i].fAngle += zer::athm::toRadians(zer::rnd::randi(-10, 10));

				float fNextAgentY = agents[i].fY + sin(agents[i].fAngle) * fItemSpeed;
				float fNextAgentX = agents[i].fX + cos(agents[i].fAngle) * fItemSpeed;
				
				if (zer::athm::pointInside2dMatrix(mWH, mWW, fNextAgentY, fNextAgentX))
				{
					agents[i].fY = fNextAgentY;
					agents[i].fX = fNextAgentX;
				}
				else
					agents[i].fAngle += zer::athm::toRadians(180);
			}
		}

		texture.update(ui8Pixmap.start());
		window.draw(sf::Sprite(texture));

		window.display();

		if (bNeedToUpdateConsole)
		{
			displayConsoleInformation(dCfg, bSimulationStarted);
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
	zer::rnd::init();

	zer::dict<std::string, float> dCfg = readConfig(msConfigPath);
	
	return loop(window, dCfg);
}