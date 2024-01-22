#include "cmos.h"
#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "memory.h"
#include "monitor.h"
#include "lexer.h"
#include "timer.h"

void outputToddOS() {
	Monitor::printf("%CrWelcome to %Cb1%CfcT%Cfeo%Cfad%Cfbd%Cf9O%CfdS%Cr\n");
}

void outputHeader() {
	for (int i = 0; i < 80; i++) {
		Monitor::writeChar('=');
	}
	Monitor::writeChar('\n');
}

void outputPassFail(bool value) {
	if (value) {
		Monitor::printf("%CfaPASS%Cr");
	} else {
		Monitor::printf("%CfcFAIL%Cr");
	}
}

void multibootInfo(MultiBoot* mboot) {
	Monitor::printf("Flags: %b\n", mboot->flags);
	Monitor::writeString("Memory Information: ");
	outputPassFail(mboot->memPresent());
	if (mboot->memPresent()) {
		Monitor::printf("\n| Lower Memory Limit: %dKB\n", mboot->memLower);
		Monitor::printf("| Upper Memory Limit: %dKB\n", mboot->memUpper);
	}
	Monitor::writeString("Boot Device: ");
	outputPassFail(mboot->bootDevicePresent());
	if (mboot->bootDevicePresent()) {
		Monitor::printf("\n| Boot Device: 0x%x", mboot->bootDevice);
	}
	Monitor::writeString("\nCMD Line: ");
	outputPassFail(mboot->cmdLinePresent());
	if (mboot->bootDevicePresent()) {
		Monitor::printf("\n| Command Line: %s", mboot->cmdLine);
	}
	Monitor::writeString("\nBoot Modules: ");
	outputPassFail(mboot->modsPresent());
	if (mboot->modsPresent()) {
		Monitor::printf("\n| #: %d", mboot->modsCount);
		if (mboot->modsCount > 0) {
			halt();
		}
	}
	Monitor::writeChar('\n');
}

const i8* tokenTypeToString(Lisp::TokenType tokenType) {
	switch (tokenType) {
	case Lisp::TokenType::LEFT:
		return "LEFT";
	case Lisp::TokenType::RIGHT:
		return "RIGHT";
	case Lisp::TokenType::PATH:
		return "PATH";
	case Lisp::TokenType::DOT:
		return "DOT";
	case Lisp::TokenType::QUOTE_LEFT:
		return "QUOTE_LEFT";
	case Lisp::TokenType::QUOTE_SYMBOL:
		return "QUOTE_SYMBOL";
	case Lisp::TokenType::INT:
		return "INT";
	case Lisp::TokenType::SYMBOL:
		return "SYMBOL";
	case Lisp::TokenType::STRING:
		return "STRING";
	case Lisp::TokenType::EOF:
		return "EOF";
	default:
		return "???";
	}
}

void outputToken(Lisp::Token const& token) {
	if (token.type == Lisp::TokenType::EOF) {
		Monitor::writeString("EOF\n");
		return;
	}
	i8* buf = Memory::kmalloc<i8>(token.end - token.start + 1);
	memcpy(token.start, buf, token.end - token.start);
	buf[token.end - token.start] = '\0';
	Monitor::printf("%s '%s'\n", tokenTypeToString(token.type), buf);
	Memory::kfree(buf);
}

#include "lex_test.h"
#include "parser.h"
#include "exe.h"
#include "lexer.h"

static i8 buf[256];

extern "C" {
	int kmain(MultiBoot* mboot, u32 initialStack) {
		initialEsp = initialStack;
		GDT::init();
		IDT::init();
		Monitor::init();
		asm volatile("sti");
		Timer::init(1000);
		Memory::init(mboot->memUpper * 1024);
		Monitor::printf("Date: %d-%d-%d\nTime: %d:%d:%d\n", (u32)CMOS::year(), (u32)CMOS::month(), (u32)CMOS::day(), (u32)CMOS::hours(), (u32)CMOS::minutes(), (u32)CMOS::seconds());
		Keyboard::init();

		Lisp::EAST::Scope scope;
		scope.init();

		memset(buf, 0, 256);
		u32 bufI = 0;
		u32 balance = 0;

		Monitor::writeString("=> ");
		while (true) {
			i8 key = Keyboard::scan();
			if (key <= 0) {
				continue;
			}
			if (key >= 'A' && key <= 'Z') {
				continue;
			}
			i8 translated = Keyboard::translateCode(key);
			Monitor::writeChar(translated);
			if (translated == '\n' && balance == 0) {
				bufI = 0;
				const i8* toParse = buf;
				Lisp::AST::ListContents* lc = Lisp::parse(&toParse);
				scope.exec(lc);
				memset(buf, 0, 256);
				Monitor::writeString("=> ");
			} else if (translated == '\n') {
				Monitor::writeString(".. ");
			} else if (translated == '\b') {
				if (buf[bufI - 1] == '(') {
					balance--;
				} else if (buf[bufI - 1] == ')') {
					balance++;
				}
				bufI--;
			} else {
				if (translated == '(') {
					balance++;
				}
				if (translated == ')') {
					balance--;
				}
				buf[bufI++] = translated;
			}
		}
		halt();
		return 0;
	}
}
