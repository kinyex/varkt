

#include<iostream>
#include<vector>
#include<cmath>
#include<algorithm>
#include<functional>

using namespace std;

class System;

const double g = 9.8;
const double G = 6.6743e-11;
const double PI = 3.14159265359;

double airDensitySpreadsheet[70] = {
	0, 1.2250, 500, 1.1673, 1000, 1.1117, 1500, 1.0581, 2000, 1.0065, 2500, 0.9569, 3000, 0.9093, 4000, 0.8194, 5000, 0.7365,
	6000, 0.6601, 7000, 0.59, 8000, 0.5258, 9000, 0.4671, 10'000, 0.4135, 11'000, 0.3648, 12'000, 0.3119, 14'000, 0.2279,
	16'000, 0.1665, 18'000, 0.1216, 20'000, 0.0889, 24'000, 0.0469, 28'000, 0.0251, 32'000, 0.0136,
	36'000, 7.26e-3, 40'000, 4.00e-3, 50'000, 1.03e-3,
	60'000, 3.00e-4, 80'000, 1.85e-5, 100'000, 5.55e-7,
	150'000, 2.00e-9, 200'000, 2.52e-10, 300'000, 1.92e-11,
	500'000, 5.21e-13, 700'000, 3.07e-14, 1'000'000, 3.56e-15
};

long double getAirDensity(long double altitude) {
	int len = sizeof(airDensitySpreadsheet) / sizeof(*airDensitySpreadsheet);
	if (altitude <= airDensitySpreadsheet[0]) return airDensitySpreadsheet[1];
	if (altitude >= airDensitySpreadsheet[len - 2]) return airDensitySpreadsheet[len - 1];
	for(size_t i = 2; i < len; i += 2) {
		if (altitude <= airDensitySpreadsheet[i]) {
			long double p[2] = {airDensitySpreadsheet[i-2], airDensitySpreadsheet[i-1]};
			long double k = (altitude - p[0]) / (airDensitySpreadsheet[i] - p[0]);
			return p[1] * (1 - k) + airDensitySpreadsheet[i + 1] * k;
		}
	}
	return 0;
}

double angleSpreadsheet[94] = {0, 0, 15, 5.55, 25, 22.49, 50, 33.67, 75, 48.08, 100, 53.98, 125, 57.82, 150, 60.23, 200, 65.56, 300, 66.65, 400, 72.3, 550, 89.9, 1e12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1e12, 90};

long double getAngle(long double time) {
	int len = sizeof(angleSpreadsheet) / sizeof(*angleSpreadsheet);
	if (time <= angleSpreadsheet[0]) return angleSpreadsheet[1];
	if (time >= angleSpreadsheet[len - 2]) return angleSpreadsheet[len - 1];
	for(size_t i = 2; i < len; i += 2) {
		if (time <= angleSpreadsheet[i]) {
			long double p[2] = {angleSpreadsheet[i-2], angleSpreadsheet[i-1]};
			long double k = (time - p[0]) / (angleSpreadsheet[i] - p[0]);
			return p[1] * (1 - k) + angleSpreadsheet[i + 1] * k;
		}
	}
	return 0;
}

double throttlingSpreadsheet[50] = {0, .975, 25, .975, 25.5, 1, 53, 1, 53.1, .8, 59.9, .8, 60, .9, 59, .9, 109, .9, 110, .94, 134, .94, 147.5, .8, 150, 0, 158, 0, 164, .985, 490, .985, 521.5, .737, 525, 0, 3000, 0, 3000.1, 1, 3001.4, 1, 3001.5, 0, 3600, 0, 3601, 1, 999999999, 0};

double getThrottling(long double time) {
	int len = sizeof(throttlingSpreadsheet) / sizeof(*throttlingSpreadsheet);
	if (time <= throttlingSpreadsheet[0]) return throttlingSpreadsheet[1];
	if (time >= throttlingSpreadsheet[len - 2]) return throttlingSpreadsheet[len - 1];
	for(size_t i = 2; i < len; i += 2) {
		if (time <= throttlingSpreadsheet[i]) {
			long double p[2] = {throttlingSpreadsheet[i-2], throttlingSpreadsheet[i-1]};
			long double k = (time - p[0]) / (throttlingSpreadsheet[i] - p[0]);
			return p[1] * (1 - k) + throttlingSpreadsheet[i + 1] * k;
		}
	}
	return 0;
}

class Vec2 {
	public:
		long double x;
		long double y;

		Vec2(long double x, long double y) : x(x), y(y) {}

		long double length() {
			return sqrt(x*x + y*y);
		}

		Vec2 normalized() {
			return *this / length();
		}

		Vec2 negative() {
			return *this * (-1);
		}

		Vec2 rotated(long double angle) {
			return Vec2(x * cos(angle) - y * sin(angle), x * sin(angle) + y * cos(angle));
		}
	
	Vec2 operator + (const Vec2& other) const {
		return Vec2(x + other.x, y + other.y);
	}
	
	Vec2 operator + (const long double& other) const {
		return Vec2(x + other, y + other);
	}
	
	Vec2& operator += (const Vec2& other) {
		x += other.x;
		y += other.y;
		return *this;
	}
	
	Vec2 operator - (const Vec2& other) const {
		return Vec2(x - other.x, y - other.y);
	}
	
	Vec2 operator - (const long double& other) const {
		return Vec2(x - other, y - other);
	}
	
	Vec2& operator -= (const Vec2& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}
	
	Vec2 operator * (const Vec2& other) const {
		return Vec2(x * other.x, y * other.y);
	}
	
	Vec2 operator * (const long double& other) const {
		return Vec2(x * other, y * other);
	}
	
	Vec2& operator *= (const Vec2& other) {
		x *= other.x;
		y *= other.y;
		return *this;
	}
	
	Vec2 operator / (const Vec2& other) const {
		if (other.x == 0 || other.y == 0) return Vec2::zero();
		return Vec2(x / other.x, y / other.y);
	}
	
	Vec2 operator / (const long double& other) const {
		if (other == 0.) return Vec2::zero();
		return Vec2(x / other, y / other);
	}
	
	Vec2& operator /= (const Vec2& other) {
		if (other.x == 0 || other.y == 0) {
			x = y = 0;
			return *this;
		}
		x /= other.x;
		y /= other.y;
		return *this;
	}

	static Vec2 zero() {
		return Vec2(0., 0.);
	}

	static long double distance(Vec2 a, Vec2 b) {
		return (a - b).length();
	}
};

struct CosmicBody {
	double mass;
	double radius;
	Vec2 position;

	function<double(double altitude)> getAirDensity;
};

CosmicBody Earth{5.9726 * pow(10, 24), 6'378'000, Vec2(0, -6'378'000), getAirDensity};

double getEarthRotationSpeedAtPoint(Vec2 pointPosition, double latitude = 0) {
	return (Earth.position - pointPosition).length() * 2 * PI * cos(latitude) / (24 * 60 * 60);
}

class RocketEngine {
	private:
		double IspASL;
		double IspVac;
		double thrustASL;
		double thrustVac;
		double fuelConsumptionRatio;

	public:
		double getThrust(double airDensity) {
			double k = min(max(airDensity / 1.225, 0.0), 1.0);
			return thrustASL * k + thrustVac * (1 - k);
		}

		double getFuelConsumption(double airDensity) {
			double k = min(max(airDensity / 1.225, 0.0), 1.0);
			return getThrust(airDensity) * fuelConsumptionRatio / (IspASL * k + IspVac * (1 - k)) / g;
		}

		RocketEngine(double IspASL, double IspVac, double thrustASL, double thrustVac, double fuelConsumptionRatio = 1) :
		IspASL(IspASL), IspVac(IspVac), thrustASL(thrustASL), thrustVac(thrustVac), fuelConsumptionRatio(fuelConsumptionRatio) {}
};

class RocketStage {
	private:
		RocketEngine* rocketEngine;
		int enginesQuantity;

	public:
		double dryMass;
		double fuelMass;
		double reserveFuelMass;
		double area;

		RocketStage(double dryMass, double fuelMass, double reserveFuelMass, RocketEngine* rocketEngine, int enginesQuantity, double area = 1) :
		dryMass(dryMass), fuelMass(fuelMass), reserveFuelMass(reserveFuelMass), rocketEngine(rocketEngine), enginesQuantity(enginesQuantity) {}

		double getMass() {
			return dryMass + fuelMass + reserveFuelMass;
		}

		double getThrust(double airDensity) {
			return rocketEngine->getThrust(airDensity) * enginesQuantity;
		}
		
		double getFuelConsumption(double airDensity) {
			return rocketEngine->getFuelConsumption(airDensity) * enginesQuantity;
		}
};

RocketEngine* Merlin1D = new RocketEngine(282, 311, 845'000, 937'000);
RocketEngine* Merlin1DVac = new RocketEngine(85, 348, 240'000, 981'000);
RocketEngine* IonEngine = new RocketEngine(2500, 2500, 0.17, 0.17, 0);

class Rocket {
	private:
		System* system;
		vector<RocketStage*> stages;
		double altitude = 121;
		Vec2 position = Vec2::zero();
		Vec2 speed = Vec2::zero();
		double diameter = 5;
		double cd = .33;
		double throttling = 1;
		double angle = 0;
		double area;
		double firstCosmicVelocity = 0;
		long double timer = 0;
		unsigned int currentStage = 1;
		unsigned int state = 1;

		double bufferedValues[4] = {0, 0, 0, 0};

	public:
		Rocket(System* system) : system(system) {
			stages.push_back(new RocketStage(800, 1, 0, IonEngine, 1, 1));
			stages.push_back(new RocketStage(3'900 + 18'200, 103'500, 0, Merlin1DVac, 1, pow(5. / 2, 2) * PI));
			stages.push_back(new RocketStage(26'300, 409'500, 0, Merlin1D, 9, pow(5. / 2, 2) * PI));
			area = pow(5. / 2, 2) * PI;
			position += Vec2(0, altitude);
			speed += Vec2(getEarthRotationSpeedAtPoint(position, PI * 28.5 / 180), 0);
		}

		~Rocket() {
			for (auto stage : stages) delete stage;
		}

		void update(double deltaTime);

		double getMass() {
			if (stages.empty()) return 1;
			double mass = 0;
			for(size_t i = 0; i < stages.size(); i++) {
				mass += stages[i]->getMass();
			}
			return mass;
		}

		void wait(long double time, bool override_timer = false) {
			if (override_timer) timer = time;
			else timer += time;
		}

		bool waitUntil(long double time, bool override_timer = true);

		void printReport();

		int switchStage() {
			if (!stages.empty()) stages.pop_back();
			currentStage++;
			area = stages.back()->area;
			return currentStage;
		}
};

class System {
	private:
		bool active = false;
		int iterator = 0;
		Rocket* rocket;
		int maxIterator;
		int dataCollectionIterations;
		double dataCollectionInterval;
		long double simulationTime;
		double deltaTime;
		
		bool update() {
			if (!active) return false;
			if (iterator++ > maxIterator) { stop("System completed successfully"); return false; }
			if (iterator % dataCollectionIterations == 0) collectData();
			time += deltaTime;
			rocket->update(deltaTime);
			return true;
		}

		void collectData() {
			rocket->printReport();
		}
	
	public:
		double time = 0;

		System(double deltaTime = .1, long double simulationTime = 1000, double dataCollectionInterval = 10) :
		deltaTime(deltaTime), simulationTime(simulationTime), dataCollectionInterval(dataCollectionInterval) {
			rocket = new Rocket(this);
			maxIterator = simulationTime / deltaTime;
			dataCollectionIterations = dataCollectionInterval / deltaTime;
		}

		~System() {
			delete rocket;
		}

		void start() {
			collectData();
			active = true;
			while(update()) {}
		}

		void stop(string message = "...") {
			collectData();
			active = false;
			printf("System has been stopped with message: \"%s\"\n", message.c_str());
		}
};

void Rocket::update(double deltaTime) {
	if (altitude < 0.) system->stop("Rocket has been crushed");

	timer -= deltaTime;

	angle = getAngle(system->time);
	throttling = timer <= 0 ? getThrottling(system->time) : 0;
	double airDensity = Earth.getAirDensity(altitude);

	if (currentStage == 1 && system->time >= 150) {
		switchStage();
		wait(8);
		return;
	}

	if (currentStage == 2 && system->time >= 3500) {
		switchStage();
	}

	if (currentStage == 3 && altitude >= 550'000) {
		currentStage++;
	}

	if (currentStage == 4) {
		throttling = 0;
	}

	Vec2 EarthDir = (Earth.position - position).normalized();
	Vec2 gravityForce = EarthDir * G * getMass() * Earth.mass / pow(Vec2::distance(Earth.position, position), 2);
	Vec2 localSpeed = (speed - EarthDir.rotated(PI / 2) * getEarthRotationSpeedAtPoint(position, PI * 28.5 / 180));
	Vec2 dragForce = localSpeed.normalized().negative() * airDensity * pow(localSpeed.length(), 2) * cd * area / 2;

	bufferedValues[0] = dragForce.length();
	bufferedValues[1] = throttling * (stages.empty() || stages.back()->fuelMass <= 0 ? 0 : stages.back()->getThrust(airDensity));
	
	speed += ((EarthDir.negative() * throttling * (stages.empty() || stages.back()->fuelMass <= 0 ? 0 : stages.back()->getThrust(airDensity))).rotated(-angle * PI / 180) + gravityForce + dragForce) * deltaTime / getMass();
	position += speed * deltaTime;
	altitude = Vec2::distance(position, Earth.position) - Earth.radius;

	bufferedValues[2] = speed.rotated(PI - atan2(EarthDir.x, EarthDir.y)).x;
	bufferedValues[3] = speed.rotated(PI - atan2(EarthDir.x, EarthDir.y)).y;

	if (!stages.empty()) stages.back()->fuelMass = max(0.0, stages.back()->fuelMass - stages.back()->getFuelConsumption(airDensity) * deltaTime * throttling);
	if (!stages.empty() && stages.back()->fuelMass <= 0 && stages.back()->fuelMass != -1) {
		switchStage();
		if (currentStage == 1) wait(8);
	}
}

void Rocket::printReport() {
	printf("Time: %7.2f | Mass: %9.0f | FuelMass: %9.0f | Speed: (%12.2f, %12.2f) | Position: (%14.2f, %14.2f) | Angle: %04.1f | Altitude: %11.0f | Drag: %9.0f | Thrust: %.2f\n", system->time, getMass(), (stages.empty() ? 0 : stages.back()->fuelMass), bufferedValues[2], bufferedValues[3], position.x, position.y, angle, altitude, bufferedValues[0], bufferedValues[1]);
}

bool Rocket::waitUntil(long double time, bool override_timer) {
	if (time <= system->time) return true;
	long double ntime = time - system->time;
	if (override_timer) timer = ntime;
	else timer += ntime;
	return false;
}

int main() {
	double deltaTime = .1;
	long double simulationTime = 10000;
	double dataCollectionInterval = 10;

	int setAngles = 0;

	int x = 1;
	while (x)
	{
		printf("Enter params (dt t interval time-angle_pairs_count):\n");
		scanf("%lf %Lf %lf %i", &deltaTime, &simulationTime, &dataCollectionInterval, &setAngles);
		
		if (setAngles > 0) {
			double a, b;
			int k = 0;
			for(int i = 0; i < setAngles; i++) {
				scanf("%lf %lf", &a, &b);
				angleSpreadsheet[k++] = a;
				angleSpreadsheet[k++] = b;
			}
		}

		System* system = new System(deltaTime, simulationTime, dataCollectionInterval);

		system->start();

		delete system;

		scanf("%i", &x);
	}

	return 0;

}
