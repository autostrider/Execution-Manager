#include <iostream>
#include <ctime>
#include <application_state_client.h>

using namespace std;
using namespace api;

using ApplicationState = ApplicationStateClient::ApplicationState;


class myTestApp
{
public:

	myTestApp() :
		appState(make_unique<ApplicationStateClient>())
	{}


	~myTestApp()
	{}

	double Fib(int place, int bound)
	{
		static double* lookup = nullptr;
		if (lookup == nullptr)
		{
			lookup = new double[bound];
			lookup[0] = 0;
			lookup[1] = 1;
			for (int i = 2; i < bound; ++i)
			{
				lookup[i] = lookup[i - 1] + lookup[i - 2];
			}
		}
		if (place < bound)
		{
			return lookup[place];
		}
		return Fib(place - 1, bound) + Fib(place - 2, bound);
	}

	int start()
	{
		int rNum = 0;
		srand(time(NULL));
		cout << "TestApp 1 is here!" << endl;

		appState->ReportApplicationState(
			ApplicationState::K_INITIALIZING);

		cout << endl << "Initializing..." << endl;
		rNum = rand() % 300;
		cout << "Bound is " << rNum << " Fibbonachi numbers." << endl;


		appState->ReportApplicationState(
			ApplicationState::K_RUNNING);

		cout << endl << "Running..." << endl;
		for (int i = 0; i < rNum; ++i)
		{
			cout << Fib(i, rNum) << " ";
		}

		appState->ReportApplicationState(
			ApplicationState::K_SHUTTINGDOWN);

		cout << endl << endl << "Terminating..." << endl;


		cout << "Best wishes" << endl;
		return EXIT_SUCCESS;
	}

private:
	unique_ptr<ApplicationStateClient> appState;
};



int main(int argc, char* argv[])
{
	myTestApp app;
	return app.start();
}