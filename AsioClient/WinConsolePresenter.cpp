#include "WinConsolePresenter.h"

WinConsolePresenter::WinConsolePresenter() {
	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

//	! возможно потребуется мьютекс !
void WinConsolePresenter::ConsoleWrite(std::string mes) {
	std::lock_guard<std::mutex> lock(mut);
	memset(input_buf, 0x00, max_len);
	CONSOLE_SCREEN_BUFFER_INFO SCInfo;
	GetConsoleScreenBufferInfo(consoleHandle, &SCInfo);
	COORD origin = SCInfo.dwCursorPosition;
	origin.X = 0;
	COORD cur_pos = SCInfo.dwCursorPosition;

	// записываем в буфер всё, что написали но не отправили
	int input_size = cur_pos.X;
	wchar_t cur_ch = '#', space = ' ';
	DWORD readed, writen;
	COORD origin_copy = origin;
	for (int i = 0; i < input_size; i++) {
		ReadConsoleOutputCharacter(consoleHandle, &cur_ch, 1, origin_copy, &readed);	// читаем символ
		WriteConsoleOutputCharacter(consoleHandle, &space, 1, origin_copy, &writen);	// сразу затираем его
		input_buf[i] = static_cast<char>(cur_ch);	// записываем считанный символ в буффер
		origin_copy.X++;	// увеличиваем позицию для чтения
	}
	
	SetConsoleCursorPosition(consoleHandle, origin);	// перемещаем курсор на начало строки консоли
	std::cout << mes << std::endl << input_buf;	// выводим наш буфер
}

std::string WinConsolePresenter::GetConsoleInput() {
	{
		std::lock_guard<std::mutex> lock(mut);
		std::cout << '>';
	}
	std::string message;
	std::getline(std::cin, message);
	// удаляем пробелы
	auto iter = find_if(message.begin(), message.end(), [](char c) { return (c != ' '); });
	if (iter != message.begin()) {
		message.erase(remove(message.begin(), iter, ' '), iter);
	}

	return message;
}