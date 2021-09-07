#include "server_session.h"

std::shared_ptr<server_session> server_session::Create() {
	std::shared_ptr<server_session> new_clt(new server_session());
	return new_clt;
}

server_session::~server_session() {
	Disconect();
}

server_session::server_session() : sock(con), started(false) {
	Pres = new WinConsolePresenter();
};

void server_session::HandlerThread() {
	con.run();
}

ip::tcp::endpoint server_session::GetAddress() {
	Pres->ConsoleWrite("Enter server address please: ");
	string address = Pres->GetConsoleInput();
	return ip::tcp::endpoint(ip::address::from_string(address), 8001);
}

void server_session::Menu() {
	string command;
	do {
		command = Pres->GetConsoleInput();
	} while (!HandleCommand(command));
}

// 1 - смогли обработать сообщение, 0 - не смогли
bool server_session::HandleCommand(string com){
	if (com == "-connect") {
		auto address = GetAddress();
		Connect(address);
		return true;
	}
	else if (com == "-help") {
		Pres->ConsoleWrite("-connect; -exit");
		return false;
	}
	else if (com == "-exit") {
		exit(0);
	}
	else {
		Pres->ConsoleWrite("unknown command: use <-help> for info.");
		return false;
	}
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
	// нужно послать серверу сообщение о том, что мы отключились
	sock.async_send(buffer("-disconect"), std::bind(&server_session::OnWrite, shared_from_this()));
	started = false;
	sock.shutdown(sock.shutdown_both);
	sock.close();
}

void server_session::Write() {
	if (!started) return;
	message = Pres->GetConsoleInput();

	if (message == "-disconect") {
		Disconect();
		Menu();
		return;
	}
	sock.async_send(buffer(message), std::bind(&server_session::OnWrite, shared_from_this()));
}

void server_session::Read() {
	if (!started) return;
	sock.async_receive(buffer(read_buf), std::bind(&server_session::OnRead, shared_from_this()));
}

void server_session::OnConnect(asio::error_code& er) {
	// проверяем, установлено ли соединение
	try {
		Pres->ConsoleWrite(sock.remote_endpoint().address().to_string());
	}
	catch (std::exception ex) {
		Pres->ConsoleWrite("connection refused");
		Menu();
		return;
	}
	// проверяем на другие ошибки
	if (!er) {
		Pres->ConsoleWrite("connected");
		started = true;
		// условно начинаем одновременно читать и отсылать
		Read();
		Write();
	}
	else {
		Pres->ConsoleWrite( er.message());
	}
}

void server_session::OnWrite() {
	memset(write_buf, 0x00, max_len);
	Write();
}

void server_session::OnRead() {
	Pres->ConsoleWrite(read_buf);
	memset(read_buf, 0x00, max_len);
	Read();
}