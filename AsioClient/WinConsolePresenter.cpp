#include "WinConsolePresenter.h"

WinConsolePresenter::WinConsolePresenter() {
	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

//	! �������� ����������� ������� !
void WinConsolePresenter::ConsoleWrite(std::string mes) {
	std::lock_guard<std::mutex> lock(mut);
	memset(input_buf, 0x00, max_len);
	CONSOLE_SCREEN_BUFFER_INFO SCInfo;
	GetConsoleScreenBufferInfo(consoleHandle, &SCInfo);
	COORD origin = SCInfo.dwCursorPosition;
	origin.X = 0;
	COORD cur_pos = SCInfo.dwCursorPosition;

	// ���������� � ����� ��, ��� �������� �� �� ���������
	int input_size = cur_pos.X;
	wchar_t cur_ch = '#', space = ' ';
	DWORD readed, writen;
	COORD origin_copy = origin;
	for (int i = 0; i < input_size; i++) {
		ReadConsoleOutputCharacter(consoleHandle, &cur_ch, 1, origin_copy, &readed);	// ������ ������
		WriteConsoleOutputCharacter(consoleHandle, &space, 1, origin_copy, &writen);	// ����� �������� ���
		input_buf[i] = static_cast<char>(cur_ch);	// ���������� ��������� ������ � ������
		origin_copy.X++;	// ����������� ������� ��� ������
	}
	
	SetConsoleCursorPosition(consoleHandle, origin);	// ���������� ������ �� ������ ������ �������
	std::cout << mes << std::endl << input_buf;	// ������� ��� �����
}

std::string WinConsolePresenter::GetConsoleInput() {
	{
		std::lock_guard<std::mutex> lock(mut);
		std::cout << '>';
	}
	std::string message;
	std::getline(std::cin, message);
	// ������� �������
	auto iter = find_if(message.begin(), message.end(), [](char c) { return (c != ' '); });
	if (iter != message.begin()) {
		message.erase(remove(message.begin(), iter, ' '), iter);
	}

	return message;
}