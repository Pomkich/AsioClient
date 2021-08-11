#include <iostream>
#include <string>
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#include <asio.hpp>
#include <functional>
#include <memory>
#include <chrono>
#include <thread>

using namespace std;
using namespace asio;

io_context con;

class server_session : public enable_shared_from_this<server_session> {
private:
	asio::ip::tcp::socket sock;
	enum { max_len = 1024 };
	char read_buf[max_len];
	char write_buf[max_len];
	bool started;
	string message;

public:
	static std::shared_ptr<server_session> Create(ip::tcp::endpoint ep, const string& mes, io_context& con) {
		std::shared_ptr<server_session> new_clt(new server_session(mes, con));
		new_clt->Connect(ep);
		return new_clt;
	}

	~server_session() { Disconect(); };

private:
	server_session(const string& mes, asio::io_context& con) : sock(con), started(false), message(mes) {
		// один для чтения, другой для записи
		std::thread t1(&server_session::HandlerThread, this);
		std::thread t2(&server_session::HandlerThread, this);
		t1.detach();
		t2.detach();
	};

	void HandlerThread() {
		con.run();
	};

	void Connect(ip::tcp::endpoint ep) {
		asio::error_code er;
		sock.async_connect(ep, std::bind(&server_session::OnConnect, shared_from_this(), er));
	}

	void Disconect() {
		started = false;
		sock.close();
	}

	void Write() {
		cout << "waiting for message: " << endl;
		getline(cin, message);
		sock.async_send(buffer(message), std::bind(&server_session::OnWrite, shared_from_this()));
	}

	void Read() {
		cout << "entered in READ" << endl;
		sock.async_receive(buffer(read_buf), std::bind(&server_session::OnRead, shared_from_this()));
	}

	// обработчик после соединения
	void OnConnect(const asio::error_code er) {
		if (!er) {
			cout << "connected" << endl;
			started = true;
			// условно начинаем одновременно читать и отсылать
			Read();
			Write();
		}
		else {
			cout << er.message();
		}
	}

	void OnWrite() {
		cout << "message was send" << endl;
		memset(write_buf, 0x00, 1024);
		Write();
	}

	void OnRead() {
		cout << "i read: " << read_buf << endl;
		memset(read_buf, 0x00, 1024);
		Read();
	}
};

int main() {

	string mes = "123";

	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);

	std::shared_ptr<server_session> clt = server_session::Create(ep, mes, con);

	this_thread::sleep_for(200000ms);

	return 0;
}