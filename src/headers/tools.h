#include "config.h"

float sense(std::vector<float>& phemap, float fY, float fX, float fAngle, float fSensorAngleOffset, float fSensorLength, float fSensorDistance);

void displayConsoleInformation(std::map<std::string, float>& cfg, bool bSimulationStarted);
void setPixelToPixmap(std::vector<uint8_t>& pixmap, int iIndex, sf::Color& cColor);
void setPixelToPixmap(std::vector<uint8_t>& pixmap, int iIndex, sf::Color&& cColor);

int init(sf::RenderWindow& window);
int main();

std::map<std::string, float> readConfig(std::string sConfigPath);

EVENT_CODE eventListener(sf::RenderWindow& window);