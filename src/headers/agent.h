class Agent
{
	public:
		float fY;
		float fX;
		float fAngle;

		Agent() {}
		inline Agent(float fY, float fX, float fAngle)
		{
			this -> fY = fY;
			this -> fX = fX;
			this -> fAngle = fAngle;
		}
};