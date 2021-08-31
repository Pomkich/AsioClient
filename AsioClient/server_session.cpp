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

server_session::server_session() : sock(con), started(false) {
	Pres = new WinConsolePresenter();
};

void server_session::HandlerThread() {
	con.run();
}

ip::tcp::endpoint server_session::GetAddress() {
	cout << "Enter server address please: ";
	string address;
	cin >> address;
	return ip::tcp::endpoint(ip::address::from_string(address), 8001);
}

void server_session::Menu() {
	string command;
	do {
		command = GetCommand();
	} while (!HandleCommand(command));
}

string server_session::GetCommand() {
	string com;
	cin >> com;
	// ������� ������� � ����� �������
	auto it = find_if(com.begin(), com.end(), [](char c) {return c != ' '; });
	if (it != com.begin()) {
		com.erase(remove(com.begin(), it, ' '), com.end());
	}
	return com;
}

// 1 - ������ ���������� ���������, 0 - �� ������
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
	// ���� ��� ������, ������ ��� ������
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
	message = Pres->GetConsoleInput();

	if (message == "-disconect") Disconect();
	sock.async_send(buffer(message), std::bind(&server_session::OnWrite, shared_from_this()));
}

void server_session::Read() {
	if (!started) return;
	sock.async_receive(buffer(read_buf), std::bind(&server_session::OnRead, shared_from_this()));
}

void server_session::OnConnect(const asio::error_code er) {
	if (!er) {
		Pres->ConsoleWrite("connected");
		started = true;
		// ������� �������� ������������ ������ � ��������
		Read();
		Write();
	}
	else {
		cout << er.message();
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