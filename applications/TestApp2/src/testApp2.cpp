#include <iostream>
#include <ctime>
#include <limits>
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

	int start()
	{
		int rLeft = 0;
		int rRight = 0;
		long int rNum = 0;
		srand(time(NULL));
		cout << "TestApp 2 is here!" << endl;

		appState->ReportApplicationState(
			ApplicationState::K_INITIALIZING);

		cout << endl << "Initializing..." << endl;
		rRight = rand() % numeric_limits<int>::max();
		rLeft = rand() % rRight;

		cout << "Boundary values were initialized: " << rLeft << " " << rRight << endl;


		appState->ReportApplicationState(
			ApplicationState::K_RUNNING);

		cout << endl << "Running..." << endl;
		rNum = rand() % rRight + rLeft;
		cout << "Random number: " << rNum << endl;

		appState->ReportApplicationState(
			ApplicationState::K_SHUTTINGDOWN);

		cout << endl << "Terminating..." << endl;


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