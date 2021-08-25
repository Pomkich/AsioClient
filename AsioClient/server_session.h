#pragma once
#include <iostream>
#include <memory>
#include <thread>
#include <functional>
#include <string>
#include <asio.hpp>

using namespace std;
using namespace asio;

class server_session : public enable_shared_from_this<server_session> {
private:
	io_context con;
	asio::ip::tcp::socket sock;
	enum { max_len = 1024 };
	char read_buf[max_len];
	char write_buf[max_len];
	bool started;
	string message;

	friend BOOL ctrl_handler(DWORD);

public:
	static std::shared_ptr<server_session> Create();
	~server_session();

private:
	server_session();
	void HandlerThread();
	static ip::tcp::endpoint GetAddress();
	void Connect(ip::tcp::endpoint ep);
	void Disconect();
	void Write();
	void Read();
	void OnConnect(const asio::error_code er);
	void OnWrite();
	void OnRead();
};

