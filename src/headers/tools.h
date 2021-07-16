#include "config.h"

float sense(zer::row<float>& fPhemap, float fY, float fX, float fAngle, float fSensorAngleOffset, float fSensorLength, float fSensorDistance);

void displayConsoleInformation(zer::dict<std::string, float>& dCfg, bool bSimulationStarted);
void setPixelToPixmap(zer::row<uint8_t>& ui8Pixmap, int iIndex, sf::Color& cColor);
void setPixelToPixmap(zer::row<uint8_t>& ui8Pixmap, int iIndex, sf::Color&& cColor);

int init(sf::RenderWindow& window);
int main();

zer::dict<std::string, float> readConfig(std::string sConfigPath);

EVENT_CODE eventListener(sf::RenderWindow& window);