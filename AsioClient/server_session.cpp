#include "server_session.h"

std::shared_ptr<server_session> server_session::Create() {
	std::shared_ptr<server_session> new_clt(new server_session());
	ip::tcp::endpoint ep = GetAddress();
	new_clt->Connect(ep);
	return new_clt;
}

server_session::~server_session() {
	Disconect();
}

server_session::server_session() : sock(con), started(false) {};

void server_session::HandlerThread() {
	con.run();
}

ip::tcp::endpoint server_session::GetAddress() {
	cout << "Enter server address please: ";
	string address;
	cin >> address;
	return ip::tcp::endpoint(ip::address::from_string(address), 8001);
}

void server_session::Connect(ip::tcp::endpoint ep) {
	// один для чтения, другой для записи
	std::thread t1(&server_session::HandlerThread, this);
	std::thread t2(&server_session::HandlerThread, this);
	t1.detach();
	t2.detach();

	asio::error_code er;
	sock.async_connect(ep, std::bind(&server_session::OnConnect, shared_from_this(), er));
}

void server_session::Disconect() {
	started = false;
}

void server_session::Write() {
	if (!started) return;
	cout << ">";
	getline(cin, message);
	// удаляем пробелы
	auto iter = find_if(message.begin(), message.end(), [](char c) { return (c != ' '); });
	if (iter != message.begin()) {
		message.erase(remove(message.begin(), iter, ' '), iter);
	}

	if (message == "-disconect") Disconect();
	sock.async_send(buffer(message), std::bind(&server_session::OnWrite, shared_from_this()));
}

void server_session::Read() {
	if (!started) return;
	sock.async_receive(buffer(read_buf), std::bind(&server_session::OnRead, shared_from_this()));
}

void server_session::OnConnect(const asio::error_code er) {
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

void server_session::OnWrite() {
	memset(write_buf, 0x00, 1024);
	Write();
}

void server_session::OnRead() {
	cout << read_buf << endl;
	memset(read_buf, 0x00, 1024);
	Read();
}