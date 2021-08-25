#include <iostream>
#include <string>

#include <asio.hpp>
#include <functional>
#include <memory>
#include <chrono>
#include <thread>
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

	this_thread::sleep_for(200000ms);

	return 0;
}