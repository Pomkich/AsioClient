#include <iostream>
#include <string>
#include <asio.hpp>
#include <functional>
#include <memory>
#include <chrono>
#include <thread>
#include <condition_variable>
#include "server_session.h"

using namespace std;
using namespace asio;

std::shared_ptr<server_session> clt = server_session::Create();


BOOL ctrl_handler(DWORD event)
{
	if (event == CTRL_CLOSE_EVENT && clt->started) {
		clt->sock.async_send(buffer("-disconect"), std::bind(&server_session::Disconect, clt));
		return TRUE;
	}
	return FALSE;
}

int main() {

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)(ctrl_handler), TRUE);

	clt->Menu();

	// лочим этот поток до тех пор, пока выполняется приложение
	mutex mt;
	condition_variable cv;
	unique_lock<mutex> lock(mt);
	cv.wait(lock);

	return 0;
}