#include <iostream>
#include <string>
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#include <asio.hpp>
#include <functional>
#include <memory>

using namespace std;
using namespace asio;

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

private:
	server_session(const string& mes, asio::io_context& con) : sock(con), started(true), message(mes) {};

	void Connect(ip::tcp::endpoint ep) {
		asio::error_code er;
		sock.async_connect(ep, std::bind(&server_session::OnConnect, shared_from_this(), er));
	}

	// обработчик после соединения
	void OnConnect(const asio::error_code er) {;
		if (!er) {
			cout << "connected" << endl;
			cin >> message;
		}
		else {
			cout << er.message();
		}
	}
};

int main() {
	io_context con;
	string mes = "123";

	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);

	std::shared_ptr<server_session> clt = server_session::Create(ep, mes, con);

	con.run();

}